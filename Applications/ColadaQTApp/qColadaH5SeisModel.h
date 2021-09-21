#ifndef __qColadaH5SeisModel_h
#define __qColadaH5SeisModel_h

// Colada includes
#include "qColadaAppExport.h"
#include "qColadaH5Model.h"

class qColadaH5SeisModelPrivate;
class H5Seis;
class H5SeisContainer;

class Q_COLADA_APP_EXPORT qColadaH5SeisModel : public qColadaH5Model {
  Q_OBJECT

public:
  typedef qColadaH5Model Superclass;

  explicit qColadaH5SeisModel(QString title, QObject *parent = nullptr);
  virtual ~qColadaH5SeisModel() override;

  virtual QVariant data(const QModelIndex &index,
                        int role = Qt::DisplayRole) const override;

  virtual bool hasChildren(const QModelIndex &parent) const override;
  virtual void fetchMore(const QModelIndex &parent) override;

  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

  virtual bool canAddH5File(h5gt::File file) const override;

  H5Seis *seisFromItem(qColadaH5Item *item) const;
  H5SeisContainer *seisCntFromItem(qColadaH5Item *item) const;

protected:
  explicit qColadaH5SeisModel(qColadaH5SeisModelPrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5SeisModel);
  Q_DISABLE_COPY(qColadaH5SeisModel);
};

#endif
