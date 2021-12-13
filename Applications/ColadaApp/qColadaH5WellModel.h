#ifndef __qColadaH5WellModel_h
#define __qColadaH5WellModel_h

// Colada includes
#include "qColadaAppExport.h"
#include "qColadaH5Model.h"

class qColadaH5WellModelPrivate;
class H5Well;
class H5WellContainer;
class H5DevCurve;
class H5LogCurve;

class Q_COLADA_APP_EXPORT qColadaH5WellModel : public qColadaH5Model {
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef qColadaH5Model Superclass;

  explicit qColadaH5WellModel(QObject *parent = nullptr);
  explicit qColadaH5WellModel(const QString &title, QObject *parent = nullptr);
  virtual ~qColadaH5WellModel() override;

public slots:
  virtual QVariant data(const QModelIndex &index,
                        int role = Qt::DisplayRole) const override;

  virtual bool hasChildren(const QModelIndex &parent) const override;
  virtual void fetchMore(const QModelIndex &parent) override;

  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

  virtual bool canAddH5File(h5gt::File file) const override;

  H5Well *wellFromItem(qColadaH5Item *item) const;
  H5WellContainer *wellCntFromItem(qColadaH5Item *item) const;
  H5DevCurve *devCurveFromItem(qColadaH5Item *item) const;
  H5LogCurve *logCurveFromItem(qColadaH5Item *item) const;

  virtual void onMRMLSceneNodeAdded(vtkObject*, vtkObject* node) override;

  void onMarkupPointAdded(vtkObject* caller, int pid);
  void onMarkupPointRemoved(vtkObject* caller, int pid);

protected:
  void getContainerAndObjNamesFromControlPointDesription(
      const QString& description,
      QString& fileName, QString& objName);

protected:
  explicit qColadaH5WellModel(qColadaH5WellModelPrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5WellModel);
  Q_DISABLE_COPY(qColadaH5WellModel);
};

#endif
