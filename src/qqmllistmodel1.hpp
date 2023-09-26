#pragma noce
#include <QAbstractListModel>
#include <QObject>
#include <QQmlListProperty>


class QmlList : public QAbstractListModel {
    Q_OBJECT
            QList<QObject *> _data;

    Q_PROPERTY(int size READ size NOTIFY sizeChanged)
    Q_PROPERTY(QQmlListProperty<QObject> content READ content)
    Q_PROPERTY(QObject * parent READ parent WRITE setParent)
    Q_CLASSINFO("DefaultProperty", "content")
public:
    explicit QmlList(QObject *parent = nullptr) : QAbstractListModel(parent) { }
    [[nodiscard]] int rowCount(const QModelIndex &p) const override { Q_UNUSED(p) return _data.size(); }
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const {
        Q_UNUSED(role)
        return QVariant::fromValue(_data[index.row()]);
    }
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override {
        static QHash<int, QByteArray> roles = { { Qt::UserRole + 1, "object" } };
        return roles;
    }
    [[nodiscard]] int size() const { return _data.size(); }
    QQmlListProperty<QObject> content() { return {this, &_data}; }

public slots:
    void add(QObject * o) { insert(o, _data.size()); }

    void insert(QObject * o, int i) {
        if (i < 0 || i > _data.size()) i = _data.size();
        beginInsertRows(QModelIndex(), i, i);
        _data.insert(i, o);
        o->setParent(this);
        emit sizeChanged();
        endInsertRows();
    }

    QObject * take(int i) {
        if ((i > -1) && (i < _data.size())) {
            beginRemoveRows(QModelIndex(), i, i);
            QObject * o = _data.takeAt(i);
            o->setParent(nullptr);
            emit sizeChanged();
            endRemoveRows();
            return o;
        } else qDebug() << "ERROR: take() failed - object out of bounds!";
        return nullptr;
    }

    QObject * get(int i) {
        if ((i > -1) && (i < _data.size())) return _data[i];
        else  qDebug() << "ERROR: get() failed - object out of bounds!";
        return nullptr;
    }

    void internalChange(QObject * o) { // added to force sort/filter reevaluation
        int i = _data.indexOf(o);
        if (i == -1) {
            qDebug() << "internal change failed, obj not found";
            return;
        } else {
            dataChanged(index(i), index(i));
        }
    }

    signals:
            void sizeChanged();
};