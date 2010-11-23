#include "sp/spFFT_oourafft.h"
#include "oourafft/ooura.h"

#define MAX(x,y) ((x) > (y) ? (x) : (y))

namespace Torch {

  /////////////////////////////////////////////////////////////////////////
  // Constructor
  spFFT_oourafft::spFFT_oourafft(bool inverse_)
    :	spCore()
  {
    inverse = inverse_;

    R = new FloatTensor;
    I = new FloatTensor;
  }

  /////////////////////////////////////////////////////////////////////////
  // Destructor
  spFFT_oourafft::~spFFT_oourafft()
  {
    delete I;
    delete R;
  }

  //////////////////////////////////////////////////////////////////////////
  // Check if the input tensor has the right dimensions and type
  bool spFFT_oourafft::checkInput(const Tensor& input) const
  {
    // Accept only tensors of Torch::Float
    if (input.getDatatype() != Tensor::Float) return false;


    /*
       input	output
       forward	1D 	2D
       inverse	2D	1D
       forward	2D	3D
       inverse	3D	2D
    */

    if (input.nDimension() == 1)
    {
      //print("spFFT_oourafft::checkInput() assuming FFT 1D ...\n");

      if(inverse)
      {
        warning("spFFT_oourafft(): impossible to handle inverse mode with 1D input tensor.");
        return false;
      }

      int N_ = input.size(0);

      unsigned int nn = nexthigher(N_); 

      if(N_ != (int) nn)
      {
        warning("spFFT_oourafft(): size(0) is not a power of 2.");
        return false;
      }
    }

    if (input.nDimension() == 2)
    {
      if(inverse)
      {
        //print("spFFT_oourafft::checkInput() assuming inverse FFT 1D ...\n");

        int N_ = input.size(0);
        unsigned int nn = nexthigher(N_); 
        if(N_ != (int) nn)
        {
          warning("spFFT_oourafft(): size(0) is not a power of 2.");
          return false;
        }
      }
      else
      {
        //print("spFFT_oourafft::checkInput() assuming FFT 2D ...\n");

        int N_ = input.size(0);
        unsigned int nn = nexthigher(N_); 
        if(N_ != (int) nn)
        {
          warning("spFFT_oourafft(): size(0) is not a power of 2.");
          return false;
        }
        N_ = input.size(1);
        nn = nexthigher(N_); 
        if(N_ != (int) nn)
        {
          warning("spFFT_oourafft(): size(1) is not a power of 2.");
          return false;
        }
      }
    }

    if (input.nDimension() == 3)
    {
      //print("spFFT_oourafft::checkInput() assuming inverse FFT 2D ...\n");

      if(inverse == false)
      {
        warning("spFFT_oourafft(): impossible to handle forward mode with 3D input tensor.");
        return false;
      }

      if(input.size(2) != 2)
      {
        warning("spFFT_oourafft(): size(2) is not 2 (necessary to handle real and imag parts).");
        return false;
      }

      int N_ = input.size(0);
      unsigned int nn = nexthigher(N_); 
      if(N_ != (int) nn)
      {
        warning("spFFT_oourafft(): size(0) is not a power of 2.");
        return false;
      }
      N_ = input.size(1);
      nn = nexthigher(N_); 
      if(N_ != (int) nn)
      {
        warning("spFFT_oourafft(): size(1) is not a power of 2.");
        return false;
      }
    }

    // OK
    return true;
  }

  /////////////////////////////////////////////////////////////////////////
  // Allocate (if needed) the output tensors given the input tensor dimensions
  bool spFFT_oourafft::allocateOutput(const Tensor& input)
  {
    if (	m_output == 0 )
    {
      cleanup();

      if (input.nDimension() == 1)
      {
        //print("spFFT_oourafft::allocateOutput() assuming FFT 1D ...\n");

        N = input.size(0);

        m_n_outputs = 1;
        m_output = new Tensor*[m_n_outputs];
        m_output[0] = new FloatTensor(N, 2);
      }
      else if (input.nDimension() == 2)
      {
        if(inverse)
        {
          //print("spFFT_oourafft::allocateOutput() assuming inverse FFT 1D ...\n");

          N = input.size(0);

          m_n_outputs = 1;
          m_output = new Tensor*[m_n_outputs];
          m_output[0] = new FloatTensor(N);
        }
        else
        {
          //print("spFFT_oourafft::allocateOutput() assuming FFT 2D ...\n");

          H = input.size(0);
          W = input.size(1);

          m_n_outputs = 1;
          m_output = new Tensor*[m_n_outputs];
          m_output[0] = new FloatTensor(H,W,2);
        }
      }
      else if (input.nDimension() == 3)
      {
        //print("spFFT_oourafft::allocateOutput() assuming inverse FFT 2D ...\n");

        H = input.size(0);
        W = input.size(1);

        m_n_outputs = 1;
        m_output = new Tensor*[m_n_outputs];
        m_output[0] = new FloatTensor(H,W);
      }
    }

    return true;
  }

  /////////////////////////////////////////////////////////////////////////
  // Process some input tensor (the input is checked, the outputs are allocated)
  bool spFFT_oourafft::processInput(const Tensor& input)
  {
    const FloatTensor* t_input = (FloatTensor*)&input;

    if (input.nDimension() == 1)
    {
      FloatTensor *RI = new FloatTensor(N);

      RI->copy(t_input);

#ifdef HAVE_OOURAFFT
      // Workspace of Ooura FFT
      int *ip;
      double *w;
      double *a;

      // Alloc workspace
      a = alloc_1d_double(2*N);
      ip = alloc_1d_int(2 + (int) sqrt(N) + 1);
      w = alloc_1d_double(N / 2 + 1);

      // Init workspace
      ip[0] = 0; // Possible speed up here as ip[0] == 0 is used to init the rest of ip and the workspace !!

      for(int i=0; i < N; i++) {
        a[2*i] = (*RI)(i);
        a[2*i+1] = 0.0;
      }

      // Complex Discrete Fourier Transform routine
      cdft(2*N, -1, a, ip, w);

      //
      FloatTensor *F = (FloatTensor *) m_output[0];
      for(int i=0; i < N; i++)
      {
        (*F)(i,0) = a[2*i];
        (*F)(i,1) = a[2*i+1];
      }

      // Free workspace
      free_1d_double(a);
      free_1d_int(ip);
      free_1d_double(w);
#endif

      delete RI;
    }
    else if (input.nDimension() == 2)
    {
      if(inverse)
      {
        R->select(t_input, 1, 0);
        I->select(t_input, 1, 1);

#ifdef HAVE_OOURAFFT
        // Workspace for Ooura FFT
        int *ip;
        double *w;
        double *a;

        // Alloc workspace
        a = alloc_1d_double(2*N);
        ip = alloc_1d_int(2 + (int) sqrt(N) + 1);
        w = alloc_1d_double(N / 2 + 1);

        // Init workspace
        ip[0] = 0;
        for(int i=0; i < N; i++)
        {
          a[2*i] = (*R)(i);
          a[2*i+1] = (*I)(i);
        }

        // Complex Discrete Fourier Transform routine (in inverse mode)
        cdft(2*N, 1, a, ip, w);

        // Return real part of the FFT only!
        FloatTensor *F = (FloatTensor *) m_output[0];
        for(int i=0; i < N; i++)
          (*F)(i) = a[2*i] / N;

        // Free workspace
        free_1d_int(ip);
        free_1d_double(w);
        free_1d_double(a);
#endif
      }
      else
      {
        FloatTensor *RI = new FloatTensor(H, W);

        RI->copy(t_input); 

#ifdef HAVE_OOURAFFT
        // Workspace for Ooura FFT
        int *ip;
        double *w;
        double **a;
        int n;

        // Alloc workspace
        a = alloc_2d_double(H, 2*W);
        n = MAX(H, W);
        ip = alloc_1d_int(2 + (int) sqrt(n) + 1);
        n = MAX(H/2+1, W/2+1);
        w = alloc_1d_double(n);

        // Init workspace
        ip[0] = 0;

        for(int i = 0 ; i < H ; i++)
        {
          for(int j = 0 ; j < W ; j++) {
            a[i][2*j] = (*RI)(i,j);
            a[i][2*j+1] = 0.;
          }
        }

        // Complex Discrete Fourier Transform 2D
        cdft2d(H, 2*W, -1, a, NULL, ip, w);

        FloatTensor *F = (FloatTensor *) m_output[0];
        for(int i = 0 ; i < H ; i++)
          for(int j = 0 ; j < W ; j++)
          {
            (*F)(i,j,0) = a[i][2*j];
            (*F)(i,j,1) = a[i][2*j+1];
          }


        // Free workspace
        free_1d_int(ip);
        free_1d_double(w);
        free_2d_double(a);
#endif
        delete RI;
      }
    }
    else if (input.nDimension() == 3)
    {
      if(inverse)
      {
        R->select(t_input, 2, 0);
        I->select(t_input, 2, 1);

#ifdef HAVE_OOURAFFT
        // Workspace for Ooura FFT
        int *ip;
        double *w;
        double **a;
        int n;

        // Alloc workspace
        a = alloc_2d_double(H, 2*W);
        n = MAX(H, W);
        ip = alloc_1d_int(2 + (int) sqrt(n) + 1);
        n = MAX(H/2+1, W/2+1);
        w = alloc_1d_double(n);

        // Init workspace
        ip[0] = 0;

        for(int i = 0 ; i < H ; i++)
          for(int j = 0 ; j < W ; j++)
          {
            a[i][2*j] = (*R)(i,j);
            a[i][2*j+1] = (*I)(i,j);
          }

        // Complex Discrete Fourier Transform 2D (in inverse mode)
        cdft2d(H, 2*W, 1, a, NULL, ip, w);

        // Return real part of the FFT only!
        FloatTensor *iF = (FloatTensor *) m_output[0];
        double scale = 1.0 / (H*W);
        for(int i = 0 ; i < H ; i++)
          for(int j = 0 ; j < W ; j++)
            (*iF)(i,j) = scale * a[i][2*j];

        // Free workspace
        free_1d_int(ip);
        free_1d_double(w);
        free_2d_double(a);
#endif
      }
    }

    // OK
    return true;
  }

  /////////////////////////////////////////////////////////////////////////

}
