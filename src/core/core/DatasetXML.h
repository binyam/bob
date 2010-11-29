/**
 * @file src/core/core/DatasetXML.h
 * @author <a href="mailto:Laurent.El-Shafey@idiap.ch">Laurent El Shafey</a>
 *
 * @brief A torch representation of a DatasetXML
 */

#ifndef TORCH5SPRO_CORE_DATASET_XML_H 
#define TORCH5SPRO_CORE_DATASET_XML_H

#include "core/Dataset2.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <blitz/array.h>


namespace Torch {   
  /**
   * \ingroup libcore_api
   * @{
   *
   */
  namespace core {

    /**
     * @brief The array XML class for an XML dataset
     */
    template <typename T, int dim> class ArrayXML: public Array {
      public:
        ArrayXML();
        ArrayXML(const char* filename);
        blitz::Array<T,dim>* getData() { return m_data; }
      //
      //load and save blitz::Array dumps, if data contained
      //call loader that knows how to read from file.
      //NULL pointer if no target!
      //Target == contained Array
      //template <typename T> load(const T&);
      
      private:
        blitz::Array<T,dim> *m_data;
        char *filename;
    };

    /**
     * @brief The arrayset XML class for an XML dataset
     */
    class ArraysetXML: public Arrayset {
      //
      //query/iterate over:
      //1. "Array"
    };


    /**
     * @brief The relation XML class for an XML dataset
     */
    class RelationXML: public Relation {
    };

    /**
     * @brief The rule XML class for an XML dataset
     */
    class RuleXML: public Rule {
    };

    /**
     * @brief The relationset XML class for an XML dataset
     */
    class RelationsetXML: public Relationset {
    };


    /**
     * @brief The main XML dataset class
     */
    class DatasetXML: public Dataset {
      public:
        DatasetXML();
        DatasetXML(char *filename);
        ~DatasetXML();
        //query/iterate over:
        //1. "Array"
        //2. "ArraySet"
        //3. "TargetSet"
        bool loadDataset(char *filename);

      private:
        xmlDocPtr m_doc;
    };


  }
  /**
   * @}
   */
}

#endif /* TORCH5SPRO_CORE_DATASET_H */
