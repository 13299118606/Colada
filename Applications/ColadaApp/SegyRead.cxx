// Colada includes
#include "SegyRead.h"
#include "util.h"

// GDAL includes
#include <gdal/gdal.h>
#include <gdal/gdal_priv.h>

// units includes
#include <units/units.hpp>

// magic enum includes
#include <magic_enum.hpp>

// Qt includes
#include <QProgressDialog>

// stl includes
#include <bitset>
#include <omp.h>

namespace {

template <class To, class From>
typename std::enable_if<(sizeof(To) == sizeof(From)) &&
std::is_trivially_copyable<From>::value &&
std::is_trivial<To>::value,
// this implementation requires that To is
// trivially default constructible
To>::type
// constexpr support needs compiler magic
inline bit_cast(const From &src) noexcept {
  To dst;
  std::memcpy(&dst, &src, sizeof(To));
  return dst;
}

inline float ibm2ieee(const qint32 &from) {
  std::bitset<32> bits_from(from);
  float to;
  const qint32 constnum1 = 2.130706432e+09; // hex2dec('7f000000') (Matlab)
  const qint32 constnum2 = 16777215;        // hex2dec('00ffffff') (Matlab)
  const float constnum3 = 16777216;         // 2^24

  float sgn = (1 - 2 * bits_from[31]);
  float expon = (from & constnum1) >> 24;
  float mantiss = (from & constnum2);

  to = sgn * float(pow(16, expon - 64)) * mantiss / constnum3;

  return to;
}

unsigned char ebc_to_ascii_table(unsigned char ascii) {
  unsigned char asc;

  int Tableebc2Asc[256] = {
      32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
      32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
      32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
      32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
      32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  91,
      46,  60,  40,  43,  33,  38,  32,  32,  32,  32,  32,  32,  32,  32,  32,
      93,  36,  42,  41,  59,  94,  45,  47,  32,  32,  32,  32,  32,  32,  32,
      32,  124, 44,  37,  95,  62,  63,  32,  32,  32,  32,  32,  32,  238, 160,
      161, 96,  58,  35,  64,  39,  61,  34,  230, 97,  98,  99,  100, 101, 102,
      103, 104, 105, 164, 165, 228, 163, 229, 168, 169, 106, 107, 108, 109, 110,
      111, 112, 113, 114, 170, 171, 172, 173, 174, 175, 239, 126, 115, 116, 117,
      118, 119, 120, 121, 122, 224, 225, 226, 227, 166, 162, 236, 235, 167, 232,
      237, 233, 231, 234, 158, 128, 129, 150, 132, 133, 148, 131, 123, 65,  66,
      67,  68,  69,  70,  71,  72,  73,  149, 136, 137, 138, 139, 140, 125, 74,
      75,  76,  77,  78,  79,  80,  81,  82,  141, 142, 143, 159, 144, 145, 92,
      32,  83,  84,  85,  86,  87,  88,  89,  90,  146, 147, 134, 130, 156, 155,
      48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  135, 152, 157, 153, 151,
      32};

  asc = Tableebc2Asc[ascii];
  return (asc);
}

}


SegyRead::SegyRead(QString readFile, QString &errMsg)
    : readFile(readFile), qFile(readFile) {
  init(errMsg);
}

SegyRead::~SegyRead() {}

void SegyRead::init(QString &errMsg) {
  if (!qFile.open(QIODevice::ReadOnly)) {
    errMsg = readFile + ": Error opening segy-file!";
    return;
  }
  if (qFile.size() < 3600) {
    errMsg = readFile + ": SEGY-file can't be less than 3600 bytes!";
    return;
  }
}

QString SegyRead::getSegyEndian() {
  h5geo::SegyEndian endian;
  qint16_le dataFormatCode_le;
  qint16_be dataFormatCode_be;

  qFile.seek(3224);
  qFile.read(bit_cast<char *>(&dataFormatCode_le), 2);
  qFile.seek(3224);
  qFile.read(bit_cast<char *>(&dataFormatCode_be), 2);
  if (dataFormatCode_le > 0 && dataFormatCode_le <= 8) {
    endian = h5geo::SegyEndian::Little;
  } else if (dataFormatCode_be > 0 && dataFormatCode_be <= 8) {
    endian = h5geo::SegyEndian::Big;
  } else {
    return QString();
  }

  return QString::fromStdString(std::string{magic_enum::enum_name(endian)});
}

QString SegyRead::getSegyFormat(h5geo::SegyEndian endian) {
  h5geo::SegyFormat format;
  qint16_le dataFormatCode_le;
  qint16_be dataFormatCode_be;
  qint16 dataFormatCode;

  qFile.seek(3224);
  qFile.read(bit_cast<char *>(&dataFormatCode_le), 2);
  qFile.seek(3224);
  qFile.read(bit_cast<char *>(&dataFormatCode_be), 2);

  if (endian == h5geo::SegyEndian::Little) {
    dataFormatCode = dataFormatCode_le;
  } else if (endian == h5geo::SegyEndian::Big) {
    dataFormatCode = dataFormatCode_be;
  } else {
    return QString();
  }

  if (dataFormatCode == 1) {
    format = h5geo::SegyFormat::FourByte_IBM;
  } else if (dataFormatCode == 2) {
    format = h5geo::SegyFormat::FourByte_integer;
  } else if (dataFormatCode == 5) {
    format = h5geo::SegyFormat::FourByte_IEEE;
  } else {
    return QString();
  }

  return QString::fromStdString(std::string{magic_enum::enum_name(format)});
}

QString SegyRead::getTxtEncoding() {
  h5geo::TxtEncoding encoding;
  char txtHdr[40][80];

  qFile.reset();
  qFile.read(*txtHdr, 3200);
  if (((txtHdr[0][0] == 'C') & (txtHdr[1][0] == 'C') & (txtHdr[2][0] == 'C')) |
      ((txtHdr[0][0] == ' ') & (txtHdr[1][0] == ' ') & (txtHdr[2][0] == ' '))) {
    encoding = h5geo::TxtEncoding::ASCII;
  } else if (((ebc_to_ascii_table(txtHdr[0][0]) == 'C') &
              (ebc_to_ascii_table(txtHdr[1][0]) == 'C') &
              (ebc_to_ascii_table(txtHdr[2][0]) == 'C')) |
             ((ebc_to_ascii_table(txtHdr[0][0]) == ' ') &
              (ebc_to_ascii_table(txtHdr[1][0]) == ' ') &
              (ebc_to_ascii_table(txtHdr[2][0]) == ' '))) {
    for (qint32 i = 0; i < 40; i++) {
      for (qint32 j = 0; j < 80; j++) {
        txtHdr[i][j] = ebc_to_ascii_table(txtHdr[i][j]);
      }
    }
    encoding = h5geo::TxtEncoding::EBCDIC;
  } else {
    return QString();
  }

  return QString::fromStdString(std::string{magic_enum::enum_name(encoding)});
}

void SegyRead::readTxtHdr(char txtHdr[40][80], h5geo::TxtEncoding encoding) {
  qFile.reset();
  qFile.read(*txtHdr, 3200);
  if (encoding == h5geo::TxtEncoding::EBCDIC) {
    for (qint32 i = 0; i < 40; i++) {
      for (qint32 j = 0; j < 80; j++) {
        txtHdr[i][j] = ebc_to_ascii_table(txtHdr[i][j]);
      }
    }
  }
}

void SegyRead::readBinHdr(double binHdr[30], h5geo::SegyEndian endian) {
  int binHdr4[3];
  qint16 binHdr2[27], binHdr2tmp[3];

  qFile.seek(3200);
  QDataStream in(&qFile);

  in.readRawData(bit_cast<char *>(&binHdr4), 12);
  in.readRawData(bit_cast<char *>(&binHdr2), 48);
  qFile.seek(3500);
  in.readRawData(bit_cast<char *>(&binHdr2tmp), 6);
  binHdr2[24] = binHdr2tmp[0];
  binHdr2[25] = binHdr2tmp[1];
  binHdr2[26] = binHdr2tmp[2];

  if (endian == h5geo::SegyEndian::Little) {
    qFromLittleEndian<qint32>(&binHdr4, std::size(binHdr4), &binHdr4);
    qFromLittleEndian<qint16>(&binHdr2, std::size(binHdr2), &binHdr2);
  } else if (endian == h5geo::SegyEndian::Big) {
    qFromBigEndian<qint32>(&binHdr4, std::size(binHdr4), &binHdr4);
    qFromBigEndian<qint16>(&binHdr2, std::size(binHdr2), &binHdr2);
  }

  for (int i = 0; i < 3; i++) {
    binHdr[i] = binHdr4[i];
  }
  for (int i = 3; i < 30; i++) {
    binHdr[i] = binHdr2[i - 3];
  }
}

double SegyRead::sampRate(h5geo::SegyEndian endian) {
  double binHdr[30];
  readBinHdr(binHdr, endian);

  return binHdr[5];
}

double SegyRead::nSamples(h5geo::SegyEndian endian) {
  double binHdr[30];
  readBinHdr(binHdr, endian);

  return binHdr[7];
}

quint64 SegyRead::nTraces(h5geo::SegyEndian endian) {
  int nSamp = nSamples(endian);

  return (qFile.size() - 3600) / (nSamp * 4 + 240);
}

void SegyRead::readOnlyTraces(
    Eigen::MatrixXd &HDR, Eigen::MatrixXf &TRACE,
    ReadOnlyParam &p, QString &errMsg,
    std::vector<std::string> shortHdrNames)
{
  std::vector<std::string> fullHdrNames_original, shortHdrNames_original;
  h5geo::getTraceHeaderNames(fullHdrNames_original, shortHdrNames_original);
  if (shortHdrNames.empty()){
    h5geo::getTraceHeaderNames(fullHdrNames_original, shortHdrNames);
  }

  std::vector<std::string> shortHdrNames_unique(shortHdrNames);
  sort(shortHdrNames_unique.begin(), shortHdrNames_unique.end());
  shortHdrNames_unique.erase(unique(
                               shortHdrNames_unique.begin(),
                               shortHdrNames_unique.end()),
                             shortHdrNames_unique.end());

  if (shortHdrNames.size() != shortHdrNames_unique.size()){
    errMsg = p.readFile + ": Short header names contain duplicates!";
    return;
  }

  std::vector<size_t> mapHdr2origin(shortHdrNames.size());
  for (size_t i = 0; i < shortHdrNames.size(); i++){
    ptrdiff_t ind = find(shortHdrNames_original.begin(),
                   shortHdrNames_original.end(),
                   shortHdrNames[i]) - shortHdrNames_original.begin();
    if (ind >= shortHdrNames.size()) {
      errMsg = p.readFile + ": Unable to map trace header short names!";
      return;
    }

    mapHdr2origin[i] = ind;
  }

  if (p.minTrc > p.maxTrc){
    errMsg = p.readFile + ": MIN trace is bigger than MAX trace!";
    return;
  }

  ptrdiff_t nSamp = p.binHdr[7];
  size_t bytesPerTrc = 4 * nSamp + 240;

  QFile qFile(p.readFile);
  if (!qFile.open(QIODevice::ReadOnly)) {
    errMsg = p.readFile + ": Error opening segy-file!";
    return;
  }
  if (qFile.size() < 3600) {
    errMsg = p.readFile + ": SEGY-file can't be less than 3600 bytes!";
    return;
  }

//  double coef = units::convert(
//      units::unit_from_string(p.spatialUnits),
//      units::unit_from_string(util::lengthUnits().toStdString()));

//  OGRSpatialReference srFrom;
//  srFrom.SetFromUserInput(p.crs.toUtf8());
//  srFrom.SetLinearUnitsAndUpdateParameters(p.spatialUnits.c_str(), coef);

//  OGRSpatialReference srTo = util::getCurrentProjection();
//  OGRCoordinateTransformation *coordTrans =
//      OGRCreateCoordinateTransformation(&srFrom, &srTo);

//  if (!coordTrans){
//    errMsg = p.readFile + ": Unable to create \"OGRCoordinateTransformation\". "
//"Either CRS or units is incorrect";
//    return;
//  }

//  bool doCoordTransform =
//      !srFrom.IsEmpty() && !srTo.IsEmpty() && !srFrom.IsSame(&srTo);

  qint32 *memFile_qint32 =
      bit_cast<qint32 *>(qFile.map(3600, qFile.size() - 3600));
  qint16 *memFile_qint16 =
      bit_cast<qint16 *>(qFile.map(3600, qFile.size() - 3600));
  float *memFile_float =
      bit_cast<float *>(qFile.map(3600, qFile.size() - 3600));

  HDR.resize(p.maxTrc - p.minTrc + 1, 78);
  TRACE.resize(nSamp, p.maxTrc - p.minTrc + 1);
  if (p.endian == h5geo::SegyEndian::Little) {
    /* reads HDR and TRACE */
    readDataFromLE(HDR, TRACE, memFile_qint32, memFile_qint16, memFile_float,
                   p.format, nSamp, bytesPerTrc, p.minTrc, p.maxTrc,
                   mapHdr2origin);
//    if (doCoordTransform)
//      crsHeaderCoordTranslate(coordTrans, HDR);
  } else if (p.endian == h5geo::SegyEndian::Big) {
    /* reads HDR and TRACE */
    readDataFromBE(HDR, TRACE, memFile_qint32, memFile_qint16, memFile_float,
                   p.format, nSamp, bytesPerTrc, p.minTrc, p.maxTrc,
                   mapHdr2origin);
//    if (doCoordTransform)
//      crsHeaderCoordTranslate(coordTrans, HDR);
  }

  qFile.unmap(bit_cast<uchar *>(memFile_qint32));
  qFile.unmap(bit_cast<uchar *>(memFile_qint16));
  qFile.unmap(bit_cast<uchar *>(memFile_float));
}

H5Seis *SegyRead::readTracesInHeap(
    ReadWriteParam &p,
    QString &errMsg,
    std::vector<std::string> shortHdrNames)
{
  std::vector<std::string> fullHdrNames_original, shortHdrNames_original;
  h5geo::getTraceHeaderNames(fullHdrNames_original, shortHdrNames_original);
  if (shortHdrNames.empty()){
    h5geo::getTraceHeaderNames(fullHdrNames_original, shortHdrNames);
  }

  std::vector<std::string> shortHdrNames_unique(shortHdrNames);
  sort(shortHdrNames_unique.begin(), shortHdrNames_unique.end());
  shortHdrNames_unique.erase(unique(
                               shortHdrNames_unique.begin(),
                               shortHdrNames_unique.end()),
                             shortHdrNames_unique.end());

  if (shortHdrNames.size() != shortHdrNames_unique.size()){
    errMsg = p.readFile + ": Short header names contain duplicates!";
    return nullptr;
  }

  std::vector<size_t> mapHdr2origin(shortHdrNames.size());
  for (size_t i = 0; i < shortHdrNames.size(); i++){
    ptrdiff_t ind = find(shortHdrNames_original.begin(),
                   shortHdrNames_original.end(),
                   shortHdrNames[i]) - shortHdrNames_original.begin();
    if (ind >= shortHdrNames.size()) {
      errMsg = p.readFile + ": Unable to map trace header short names!";
      return nullptr;
    }

    mapHdr2origin[i] = ind;
  }

  Eigen::MatrixXd HDR;
  Eigen::MatrixXf TRACE;
  Eigen::VectorXd minHDRVec(78), maxHDRVec(78);
  minHDRVec.fill(std::numeric_limits<double>::infinity());
  maxHDRVec.fill(-std::numeric_limits<double>::infinity());

  ptrdiff_t nSamp = p.binHdr[7];
  double sampRate = p.binHdr[5];

  QFile qFile(p.readFile);
  if (!qFile.open(QIODevice::ReadOnly)) {
    errMsg = p.readFile + ": Error opening segy-file!";
    return nullptr;
  }
  if (qFile.size() < 3600) {
    errMsg = p.readFile + ": SEGY-file can't be less than 3600 bytes!";
    return nullptr;
  }

  size_t nTrc = (qFile.size() - 3600) / (nSamp * 4 + 240);
  size_t bytesPerTrc = 4 * nSamp + 240;
  size_t fromTrc = 0;
  size_t J = p.traceHeapSize;
  size_t N = nTrc / p.traceHeapSize;

//  double coef = units::convert(
//      units::unit_from_string(p.spatialUnits),
//      units::unit_from_string(util::lengthUnits().toStdString()));

//  OGRSpatialReference srFrom;
//  srFrom.SetFromUserInput(p.crs.toUtf8());
//  srFrom.SetLinearUnitsAndUpdateParameters(p.spatialUnits.c_str(), coef);

//  OGRSpatialReference srTo = util::getCurrentProjection();
//  OGRCoordinateTransformation *coordTrans =
//      OGRCreateCoordinateTransformation(&srFrom, &srTo);

//  if (!coordTrans){
//    errMsg = p.readFile + ": Unable to create \"OGRCoordinateTransformation\". "
//"Either CRS or units is incorrect";
//    return nullptr;
//  }

//  bool doCoordTransform =
//      !srFrom.IsEmpty() && !srTo.IsEmpty() && !srFrom.IsSame(&srTo);

  std::string saveFile = p.saveFile.toStdString();
  H5SeisCnt_ptr seisCnt_ptr = H5SeisCnt_ptr(h5geo::createSeisContainerByName(
      saveFile, h5geo::CreationType::OPEN_OR_CREATE));

  if (seisCnt_ptr == nullptr) {
    errMsg = p.readFile + ": Unable to open or create seis container";
    return nullptr;
  }

  p.nTrc = nTrc;
  p.nSamp = nSamp;

  std::string seisName = p.seisName.toStdString();
  H5Seis *seis =
      seisCnt_ptr->createSeis(seisName, p, p.seisCreateType);
  if (seis == nullptr) {
    errMsg = p.readFile + ": Can't open or create seis";
    seis->Delete();
    return nullptr;
  }

  QProgressDialog progressDialog("Reading file:\t" + p.seisName, "Abort", 0, N);
  progressDialog.setWindowModality(Qt::WindowModal);
  progressDialog.setWindowTitle("Processing...");
  progressDialog.setAutoClose(false);

  qint64 progress = 0;
  if (p.endian == h5geo::SegyEndian::Little) {
    /* OMP on Windows claims that iterator to be SIGNED INTEGER type */
#pragma omp parallel for num_threads(p.nThread2use) private(HDR, TRACE, J)
    for (qint64 n = 0; n <= N; n++) {
      if (n < N) {
        J = p.traceHeapSize;
      } else {
        J = nTrc - N * p.traceHeapSize;
      }
      if (J == 0)
        continue;
      HDR.resize(J, 78);
      TRACE.resize(nSamp, J);

      qint64 memoryOffset =
          3600 + n * p.traceHeapSize * bytesPerTrc; // traceHeapSize !!! NOT J
      qint64 memorySize = J * bytesPerTrc;
      qint32 *memFile_qint32 =
          bit_cast<qint32 *>(qFile.map(memoryOffset, memorySize));
      qint16 *memFile_qint16 =
          bit_cast<qint16 *>(qFile.map(memoryOffset, memorySize));
      float *memFile_float =
          bit_cast<float *>(qFile.map(memoryOffset, memorySize));

      /* reads HDR and TRACE */
      readDataFromLE(HDR, TRACE, memFile_qint32, memFile_qint16, memFile_float,
                     p.format, nSamp, bytesPerTrc, 0, J,
                     mapHdr2origin);
//      if (doCoordTransform)
//        crsHeaderCoordTranslate(coordTrans, HDR);

      if (progressDialog.wasCanceled())
        continue;

#pragma omp critical
      {
        progressDialog.setValue(progress);
        progress++;

        seis->writeTraceHeader(HDR, fromTrc);
        seis->writeTrace(TRACE, fromTrc);
        fromTrc = fromTrc + J;

        for (ptrdiff_t i = 0; i < HDR.rows(); i++) {
          for (ptrdiff_t j = 0; j < HDR.cols(); j++) {
            minHDRVec(j) = std::min(minHDRVec(j), HDR(i, j));
            maxHDRVec(j) = std::max(maxHDRVec(j), HDR(i, j));
          }
        }
      }
      qFile.unmap(bit_cast<uchar *>(memFile_qint32));
      qFile.unmap(bit_cast<uchar *>(memFile_qint16));
      qFile.unmap(bit_cast<uchar *>(memFile_float));
    }
  } else if (p.endian == h5geo::SegyEndian::Big) {
#pragma omp parallel for num_threads(p.nThread2use) private(HDR, TRACE, J)
    for (qint64 n = 0; n <= N; n++) {
      if (n < N) {
        J = p.traceHeapSize;
      } else {
        J = nTrc - N * p.traceHeapSize;
      }
      if (J == 0)
        continue;
      HDR.resize(J, 78);
      TRACE.resize(nSamp, J);

      qint64 memoryOffset =
          3600 + n * p.traceHeapSize * bytesPerTrc; // traceHeapSize !!! NOT J
      qint64 memorySize = J * bytesPerTrc;
      qint32 *memFile_qint32 =
          bit_cast<qint32 *>(qFile.map(memoryOffset, memorySize));
      qint16 *memFile_qint16 =
          bit_cast<qint16 *>(qFile.map(memoryOffset, memorySize));
      float *memFile_float =
          bit_cast<float *>(qFile.map(memoryOffset, memorySize));

      /* reads HDR and TRACE */
      readDataFromBE(HDR, TRACE, memFile_qint32, memFile_qint16, memFile_float,
                     p.format, nSamp, bytesPerTrc, 0, J,
                     mapHdr2origin);
//      if (doCoordTransform)
//        crsHeaderCoordTranslate(coordTrans, HDR);

      if (progressDialog.wasCanceled())
        continue;

#pragma omp critical
      {
        progressDialog.setValue(progress);
        progress++;

        seis->writeTraceHeader(HDR, fromTrc);
        seis->writeTrace(TRACE, fromTrc);
        fromTrc = fromTrc + J;

        for (ptrdiff_t i = 0; i < HDR.rows(); i++) {
          for (ptrdiff_t j = 0; j < HDR.cols(); j++) {
            minHDRVec(j) = std::min(minHDRVec(j), HDR(i, j));
            maxHDRVec(j) = std::max(maxHDRVec(j), HDR(i, j));
          }
        }
      }
      qFile.unmap(bit_cast<uchar *>(memFile_qint32));
      qFile.unmap(bit_cast<uchar *>(memFile_qint16));
      qFile.unmap(bit_cast<uchar *>(memFile_float));
    }
  }

  if (progressDialog.wasCanceled()){
    errMsg = p.readFile + ": Aborted";
    seis->Delete();
    return nullptr;
  }

  // write tzt and bin headers
  seis->writeTextHeader(p.txtHdr);
  seis->writeBinHeader(p.binHdr);

  // write min/max HDR
  auto opt = seis->getTraceHeaderD();
  if (!opt.has_value()) {
    errMsg = p.readFile + ": Unable to get trace header dataset";
    seis->Delete();
    return nullptr;
  }
  opt->createAttribute<double>("min", h5gt::DataSpace(minHDRVec.size()))
      .write_raw(minHDRVec.data());
  opt->createAttribute<double>("max", h5gt::DataSpace(minHDRVec.size()))
      .write_raw(maxHDRVec.data());

  // sort headers
  QStringList hdr2sortList;
  if (p.dataType == h5geo::SeisDataType::PRESTACK) {
    hdr2sortList = QStringList(
          {"FFID", "SP", "CDP", "DSREG", "SRCX", "SRCY", "GRPX",
           "GRPY", "CDP_X", "CDP_Y", "INLINE", "XLINE"});
  } else {
    hdr2sortList = QStringList({"CDP_X", "CDP_Y", "INLINE", "XLINE"});
  }

  progressDialog.setRange(0, hdr2sortList.count());
  progress = 0;
  for (int i = 0; i < hdr2sortList.count(); i++) {
    if (progressDialog.wasCanceled()){
      errMsg = p.readFile + ": Aborted";
      seis->Delete();
      return nullptr;
    }

    progressDialog.setLabelText(p.seisName + " sorting:\t" + hdr2sortList[i]);
    progressDialog.setValue(progress);
    progress++;

    seis->addPKeySort(hdr2sortList[i].toStdString());
  }
  progressDialog.setValue(progressDialog.maximum());
  progressDialog.setLabelText("Finalizing (calculating boundary)...");
  // calculate border of area
  if (!seis->Finalize()){
    errMsg = p.readFile + ": Unable to calculate and write boundary. Check INLINE, XLINE, CDP_X, CDP_Y headers";
    seis->Delete();
    return nullptr;
  }

  seis->getH5File().flush();
  progressDialog.close();
  return seis;
}

void SegyRead::readDataFromLE(
    Eigen::MatrixXd &HDR, Eigen::MatrixXf &TRACE,
    const qint32 *memFile_qint32,
    const qint16 *memFile_qint16,
    const float *memFile_float,
    const h5geo::SegyFormat &format, const int &nSamp,
    const size_t &bytesPerTrc, const size_t &minTrc,
    const size_t &maxTrc,
    const std::vector<size_t>& mapHdr2origin)
{
  for (qint64 j = minTrc; j < maxTrc; j++) {
    for (size_t i = 0; i < 7; i++) {
      HDR(j, mapHdr2origin[i]) = qFromLittleEndian(
            memFile_qint32[j * bytesPerTrc / 4 + i]);
    }
    for (size_t i = 7; i < 11; i++) {
      HDR(j, mapHdr2origin[i]) =
          qFromLittleEndian(memFile_qint16[j * bytesPerTrc / 2 + (i - 7) + 14]);
    }
    for (size_t i = 11; i < 19; i++) {
      HDR(j, mapHdr2origin[i]) =
          qFromLittleEndian(memFile_qint32[j * bytesPerTrc / 4 + (i - 11) + 9]);
    }
    for (size_t i = 19; i < 21; i++) {
      HDR(j, mapHdr2origin[i]) = qFromLittleEndian(
          memFile_qint16[j * bytesPerTrc / 2 + (i - 19) + 34]);
    }
    for (size_t i = 21; i < 25; i++) {
      HDR(j, mapHdr2origin[i]) = qFromLittleEndian(
          memFile_qint32[j * bytesPerTrc / 4 + (i - 21) + 18]);
    }
    for (size_t i = 25; i < 71; i++) {
      HDR(j, mapHdr2origin[i]) = qFromLittleEndian(
          memFile_qint16[j * bytesPerTrc / 2 + (i - 25) + 44]);
    }
    for (size_t i = 71; i < 76; i++) {
      HDR(j, mapHdr2origin[i]) = qFromLittleEndian(
          memFile_qint32[j * bytesPerTrc / 4 + (i - 71) + 45]);
    }
    for (size_t i = 76; i < 78; i++) {
      HDR(j, mapHdr2origin[i]) = qFromLittleEndian(
          memFile_qint16[j * bytesPerTrc / 2 + (i - 76) + 100]);
    }

    if (format == h5geo::SegyFormat::FourByte_IBM) {
      for (size_t i = 0; i < nSamp; i++) {
        TRACE(i, j) = ibm2ieee(
              qFromLittleEndian(memFile_qint32[j * bytesPerTrc / 4 + i + 60]));
      }
    } else if (format == h5geo::SegyFormat::FourByte_integer) {
      for (size_t i = 0; i < nSamp; i++) {
        TRACE(i, j) =
            qFromLittleEndian(memFile_qint32[j * bytesPerTrc / 4 + i + 60]);
      }
    } else if (format == h5geo::SegyFormat::FourByte_IEEE) {
      for (size_t i = 0; i < nSamp; i++) {
        TRACE(i, j) =
            qFromLittleEndian(memFile_float[j * bytesPerTrc / 4 + i + 60]);
      }
    }
  }
}

void SegyRead::readDataFromBE(
    Eigen::MatrixXd &HDR, Eigen::MatrixXf &TRACE,
    const qint32 *memFile_qint32,
    const qint16 *memFile_qint16,
    const float *memFile_float,
    const h5geo::SegyFormat &format, const int &nSamp,
    const size_t &bytesPerTrc, const size_t &minTrc,
    const size_t &maxTrc,
    const std::vector<size_t>& mapHdr2origin)
{
  for (qint64 j = minTrc; j < maxTrc; j++) {
    for (size_t i = 0; i < 7; i++) {
      HDR(j, mapHdr2origin[i]) = qFromBigEndian(
            memFile_qint32[j * bytesPerTrc / 4 + i]);
    }
    for (size_t i = 7; i < 11; i++) {
      HDR(j, mapHdr2origin[i]) = qFromBigEndian(
            memFile_qint16[j * bytesPerTrc / 2 + (i - 7) + 14]);
    }
    for (size_t i = 11; i < 19; i++) {
      HDR(j, mapHdr2origin[i]) = qFromBigEndian(
            memFile_qint32[j * bytesPerTrc / 4 + (i - 11) + 9]);
    }
    for (size_t i = 19; i < 21; i++) {
      HDR(j, mapHdr2origin[i]) = qFromBigEndian(
            memFile_qint16[j * bytesPerTrc / 2 + (i - 19) + 34]);
    }
    for (size_t i = 21; i < 25; i++) {
      HDR(j, mapHdr2origin[i]) = qFromBigEndian(
            memFile_qint32[j * bytesPerTrc / 4 + (i - 21) + 18]);
    }
    for (size_t i = 25; i < 71; i++) {
      HDR(j, mapHdr2origin[i]) = qFromBigEndian(
            memFile_qint16[j * bytesPerTrc / 2 + (i - 25) + 44]);
    }
    for (size_t i = 71; i < 76; i++) {
      HDR(j, mapHdr2origin[i]) = qFromBigEndian(
            memFile_qint32[j * bytesPerTrc / 4 + (i - 71) + 45]);
    }
    for (size_t i = 76; i < 78; i++) {
      HDR(j, mapHdr2origin[i]) = qFromBigEndian(
            memFile_qint16[j * bytesPerTrc / 2 + (i - 76) + 100]);
    }

    if (format == h5geo::SegyFormat::FourByte_IBM) {
      for (size_t i = 0; i < nSamp; i++) {
        TRACE(i, j) = ibm2ieee(
            qFromBigEndian(memFile_qint32[j * bytesPerTrc / 4 + i + 60]));
      }
    } else if (format == h5geo::SegyFormat::FourByte_integer) {
      for (size_t i = 0; i < nSamp; i++) {
        TRACE(i, j) =
            qFromBigEndian(memFile_qint32[j * bytesPerTrc / 4 + i + 60]);
      }
    } else if (format == h5geo::SegyFormat::FourByte_IEEE) {
      for (size_t i = 0; i < nSamp; i++) {
        TRACE(i, j) =
            qFromBigEndian(memFile_float[j * bytesPerTrc / 4 + i + 60]);
      }
    }
  }
}

void SegyRead::crsHeaderCoordTranslate(
    OGRCoordinateTransformation *coordTrans, Eigen::MatrixXd &HDR)
{
  // SRCX GRPX CDP_X
  Eigen::Vector3<size_t> x({21, 23, 71});
  // SRCY GRPY CDP_Y
  Eigen::Vector3<size_t> y({22, 24, 72});

  for (size_t i = 0; i < x.size(); i++) {
    coordTrans->Transform(HDR.rows(), HDR.col(x(i)).data(),
                          HDR.col(y(i)).data());
  }
}
