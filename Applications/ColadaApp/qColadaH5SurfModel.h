#ifndef __qColadaH5SurfModel_h
#define __qColadaH5SurfModel_h

// Colada includes
#include "qColadaAppExport.h"
#include "qColadaH5Model.h"

class qColadaH5SurfModelPrivate;
class H5Surf;
class H5SurfContainer;

class Q_COLADA_APP_EXPORT qColadaH5SurfModel : public qColadaH5Model {
  Q_OBJECT

public:
  typedef qColadaH5Model Superclass;

  explicit qColadaH5SurfModel(QObject *parent = nullptr);
  explicit qColadaH5SurfModel(const QString &title,
                              QObject *parent = nullptr);
  virtual ~qColadaH5SurfModel() override;

public slots:
  virtual QVariant data(const QModelIndex &index,
                        int role = Qt::DisplayRole) const override;

  virtual bool hasChildren(const QModelIndex &parent) const override;
  virtual void fetchMore(const QModelIndex &parent) override;

  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

  virtual bool canAddH5File(h5gt::File file) const override;

  H5Surf *surfFromItem(qColadaH5Item *item) const;
  H5SurfContainer *surfCntFromItem(qColadaH5Item *item) const;

protected:
  explicit qColadaH5SurfModel(
    qColadaH5SurfModelPrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5SurfModel);
  Q_DISABLE_COPY(qColadaH5SurfModel);
};

#endif
