#ifndef _TORCHSPRO_SP_FFT_oourafft_H_
#define _TORCHSPRO_SP_FFT_oourafft_H_

#include "core/Tensor.h"
#include "sp/spCore.h"

namespace Torch
{
/**
 * \ingroup libsp_api
 * @{
 *
 */

	/////////////////////////////////////////////////////////////////////////
	// Torch::spFFT_oourafft
	//	This class is designed to perform FFT.
	//	The result is a tensor of the same storage type.
	//
	// TODO: doxygen header!
	/////////////////////////////////////////////////////////////////////////

	class spFFT_oourafft : public spCore
	{
	public:

		// Constructor
		spFFT_oourafft(bool inverse_ = false);

		// Destructor
		virtual ~spFFT_oourafft();

	protected:

		//////////////////////////////////////////////////////////

		/// Check if the input tensor has the right dimensions and type - overriden
		virtual bool		checkInput(const Tensor& input) const;

		/// Allocate (if needed) the output tensors given the input tensor dimensions - overriden
		virtual bool		allocateOutput(const Tensor& input);

		/// Process some input tensor (the input is checked, the outputs are allocated) - overriden
		virtual bool		processInput(const Tensor& input);

		//////////////////////////////////////////////////////////

	private:

		/////////////////////////////////////////////////////////////////
		// Attributes

		bool inverse;

		int N;
		int H,W;

		FloatTensor *R;
		FloatTensor *I;
	};

/**
 * @}
 */

}

#endif