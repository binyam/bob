/**
 * @file src/core/core/Dataset2.h
 * @author <a href="mailto:Laurent.El-Shafey@idiap.ch">Laurent El Shafey</a>
 *
 * @brief A torch abstract representation of a Dataset
 */

#ifndef TORCH5SPRO_CORE_DATASET_H
#define TORCH5SPRO_CORE_DATASET_H


namespace Torch {   
  /**
   * \ingroup libcore_api
   * @{
   *
   */
  namespace core {

    /**
     * @brief The array class for a dataset
     */
    class Array { //pure virtual
      //
      //load and save blitz::Array dumps, if data contained
      //call loader that knows how to read from file.
      //NULL pointer if no target!
      //Target == contained Array
      //template <typename T> load(const T&);
    };

    /**
     * @brief The arrayset class for a dataset
     */
    class Arrayset { //pure virtual
      //
      //query/iterate over:
      //1. "Array"
    };


    /**
     * @brief The relation class for a dataset
     */
    class Relation { //pure virtual
    };

    /**
     * @brief The rule class for a dataset
     */
    class Rule { //pure virtual
    };

    /**
     * @brief The relationset class for a dataset
     */
    class Relationset { //pure virtual
    };

  
    /**
     * @brief The main dataset class
     */
    class Dataset { //pure virtual
      //query/iterate over:
      //1. "Patternset"
      //2. "Cluster"
      //3. "Mapping"
      public:
        virtual bool loadDataset(char *filename) = 0;

      private:
        
    };


  }
  /**
   * @}
   */
}

#endif /* TORCH5SPRO_CORE_DATASET_H */
