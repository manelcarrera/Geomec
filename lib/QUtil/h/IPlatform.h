#ifndef _IPlatform_h_
#define _IPlatform_h_

class QString;

class IPlatform {
public:
  virtual ~IPlatform();

  static IPlatform *instance();

  virtual long processMemorySize() const = 0;
  virtual void trace(const QString &message) const = 0;

protected:
  IPlatform();

  static IPlatform *m_singleton;

private:
  IPlatform(const IPlatform &rhs);
  IPlatform &operator=(const IPlatform &rhs);
};

#endif // _IPlatform_h_
