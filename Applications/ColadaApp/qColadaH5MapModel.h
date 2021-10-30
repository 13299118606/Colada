#ifndef __qColadaH5MapModel_h
#define __qColadaH5MapModel_h

// Colada includes
#include "qColadaAppExport.h"
#include "qColadaH5Model.h"

class qColadaH5MapModelPrivate;
class H5Map;
class H5MapContainer;

class Q_COLADA_APP_EXPORT qColadaH5MapModel : public qColadaH5Model {
  Q_OBJECT

public:
  typedef qColadaH5Model Superclass;

  explicit qColadaH5MapModel(QObject *parent = nullptr);
  explicit qColadaH5MapModel(const QString &title,
                              QObject *parent = nullptr);
  virtual ~qColadaH5MapModel() override;

public slots:
  virtual QVariant data(const QModelIndex &index,
                        int role = Qt::DisplayRole) const override;

  virtual bool hasChildren(const QModelIndex &parent) const override;
  virtual void fetchMore(const QModelIndex &parent) override;

  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

  virtual bool canAddH5File(h5gt::File file) const override;

  H5Map *mapFromItem(qColadaH5Item *item) const;
  H5MapContainer *mapCntFromItem(qColadaH5Item *item) const;

protected:
  explicit qColadaH5MapModel(
    qColadaH5MapModelPrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5MapModel);
  Q_DISABLE_COPY(qColadaH5MapModel);
};

#endif
