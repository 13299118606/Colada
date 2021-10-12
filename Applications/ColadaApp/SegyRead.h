#ifndef __SegyRead_h
#define __SegyRead_h

// Qt includes
#include <QtCore>

// h5geo includes
#include <h5geo/h5core.h>
#include <h5geo/h5seis.h>
#include <h5geo/h5seiscontainer.h>

// Eigen includes
#include <Eigen/Dense>

class OGRCoordinateTransformation;

class SegyRead {
public:
  struct ReadOnlyParam {
    QString readFile, crs;
    h5geo::SegyEndian endian;
    h5geo::SegyFormat format;
    std::string spatialUnits, temporalUnits, dataUnits;
    double binHdr[30]; // 30 is the number of binary headers
    size_t minTrc, maxTrc;
  };

  struct ReadWriteParam : SeisParam {
    QString readFile, saveFile, crs, seisName;
    h5geo::TxtEncoding encoding;
    h5geo::SegyEndian endian;
    h5geo::SegyFormat format;
    char txtHdr[40][80];
    double binHdr[30]; // 30 is the number of binary headers
    h5geo::CreationType seisCreateType;
    double srd;
    size_t traceHeapSize = 10000, nThread2use = 4;
  };

  explicit SegyRead(QString readFile, QString &errMsg);
  virtual ~SegyRead();

  QString getSegyEndian();
  QString getSegyFormat(h5geo::SegyEndian endian);
  QString getTxtEncoding();

  void readTxtHdr(char txtHdr[40][80], h5geo::TxtEncoding encoding);
  void readBinHdr(double binHdr[30], h5geo::SegyEndian endian);

  double sampRate(h5geo::SegyEndian endian);
  double nSamples(h5geo::SegyEndian endian);
  quint64 nTraces(h5geo::SegyEndian endian);

  static void readOnlyTraces(Eigen::MatrixXd &HDR, Eigen::MatrixXf &TRACE,
                             ReadOnlyParam &p, QString &errMsg);
  static H5Seis *readTracesInHeap(
      ReadWriteParam &p,
      QString &errMsg);

private:
  void init(QString &errMsg);
  inline static void
  readDataFromLE(Eigen::MatrixXd &HDR, Eigen::MatrixXf &TRACE,
                 const qint32 *memFile_qint32, const qint16 *memFile_qint16,
                 const float *memFile_float, const h5geo::SegyFormat &format,
                 const int &nSamp, const size_t &bytesPerTrc,
                 const size_t &minTrc, const size_t &maxTrc);

  inline static void
  readDataFromBE(Eigen::MatrixXd &HDR, Eigen::MatrixXf &TRACE,
                 const qint32 *memFile_qint32, const qint16 *memFile_qint16,
                 const float *memFile_float, const h5geo::SegyFormat &format,
                 const int &nSamp, const size_t &bytesPerTrc,
                 const size_t &minTrc, const size_t &maxTrc);

  inline static void
  crsHeaderCoordTranslate(OGRCoordinateTransformation *coordTrans,
                          Eigen::MatrixXd &HDR);

  /// no need to make these vars 'public'
private:
  QString readFile;
  QFile qFile;
};

#endif // __SegyRead_h
