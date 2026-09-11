#ifndef _OIDI_TASK_COLLECTOR_H_
#define _OIDI_TASK_COLLECTOR_H_

// Tasks will not work on global OIDISets, but on "records". When they're finished, these records will be added to a
// collector and they can be processed afterwards to move the data to the global sets. While we could do this in
// parallel too, it should be too fast to bother with it anyway.

template <class Type> class ICollectorRecord {
protected:
  size_t m_offset;
  std::vector<Type> m_values;

public:
  ICollectorRecord(size_t offset, size_t size) : m_offset(offset) { m_values.resize(size, Type(DBL_UNDEFINED)); }

  virtual size_t getOffset() const { return m_offset; }
  virtual size_t getSize() const { return m_values.size(); }

  virtual std::vector<Type> &getValues() { return m_values; }
  virtual std::vector<size_t> *getCounts() { return 0; }
  virtual std::vector<size_t> *getInvalidCounts() { return 0; }
};

template <class Type, class Container> class ICollector {
protected:
  tbb::concurrent_vector<ICollectorRecord<Type> *> m_records;

public:
  typedef Type value_type;
  typedef std::vector<Type> value_vector_type;
  typedef std::vector<size_t> size_vector_type;
  typedef ICollectorRecord<Type> record_type;

  virtual ~ICollector() {
    for (size_t i = 0; i < m_records.size(); ++i)
      delete m_records[i];
  }

  void add(ICollectorRecord<Type> *record) // we will own these records
  {
    m_records.push_back(record);
  }

  virtual void collect(Container &) = 0;
};

template <class Type, class Container> class CCollector : public ICollector<Type, Container> {
public:
  virtual void collect(Container &container) {
    for (size_t i = 0; i < m_records.size(); ++i) {
      record_type &record = *static_cast<record_type *>(m_records[i]);

      size_t offset = record.getOffset();
      size_t size = record.getSize();

      ICollector<Type, Container>::value_vector_type &values = record.getValues();

      for (size_t j = 0; j < size; ++j) {
        container.set(offset + j, values[j]);
      }
    }
  }
};

typedef CCollector<double, OIDIScalarSetI> CScalarCollector;
typedef CCollector<MbVec3d, OIDIVectorSetI> CVectorCollector;
typedef CCollector<CTensor, OIDITensorSetI> CTensorCollector;

template <class Type> struct ValueIndexPair {
  size_t size;
  Type value[8]; // assume no more than 16 interface element nodes
  size_t index[8];

  ValueIndexPair(double init) : size(0) {
    for (size_t i = 0; i < 8; ++i) {
      value[i] = init;
      index[i] = size_t(-1);
    }
  }
};

template <class Type, class Container> class CInterfaceCollector : public ICollector<Type, Container> {
public:
  virtual void collect(Container &container) {
    for (size_t i = 0; i < m_records.size(); ++i) {
      record_type &record = *static_cast<record_type *>(m_records[i]);

      size_t offset = record.getOffset();
      size_t size = record.getSize();

      value_vector_type &values = record.getValues();

      for (size_t j = 0; j < size; ++j) {
        for (size_t k = 0; k < values[j].size; ++k) {
          if (values[j].index[k] != size_t(-1))
            container.set(values[j].index[k], values[j].value[k]);
        }
      }
    }
  }
};

typedef CInterfaceCollector<ValueIndexPair<MbVec3d>, OIDIVectorSetI> CInterfaceVectorCollector;

#endif