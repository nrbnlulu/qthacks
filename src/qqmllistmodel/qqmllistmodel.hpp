#pragma once

#include <QQmlListProperty>
#include <QVector>

#include "../utils.hpp"

template <class ObjType>
class QQmlSmartListWrapper : public QQmlListProperty<ObjType> {
public:
    typedef QVector<ObjType*> CppListType;
    typedef QQmlListProperty<ObjType> QmlListPropertyType;
    typedef QQmlSmartListWrapper<ObjType> SmartListWrapperType;

    typedef typename CppListType::const_iterator const_iterator;

    explicit QQmlSmartListWrapper(QObject* object, const int reserve = 0)
            : QmlListPropertyType(object,
                                  &m_items,
                                  &SmartListWrapperType::callbackAppend,
                                  &SmartListWrapperType::callbackCount,
                                  &SmartListWrapperType::callbackAt,
                                  &SmartListWrapperType::callbackClear) {
        if (reserve > 0) {
            m_items.reserve(reserve);
        }
    }

    const CppListType& items() const { return m_items; }

    const_iterator begin() const { return m_items.begin(); }

    const_iterator end() const { return m_items.end(); }

    const_iterator constBegin() const { return m_items.constBegin(); }

    const_iterator constEnd() const { return m_items.constEnd(); }

    static int callbackCount(QmlListPropertyType* prop) { return static_cast<CppListType*>(prop->data)->count(); }

    static void callbackClear(QmlListPropertyType* prop) { static_cast<CppListType*>(prop->data)->clear(); }

    static void callbackAppend(QmlListPropertyType* prop, ObjType* obj) {
        static_cast<CppListType*>(prop->data)->append(obj);
    }

    static ObjType* callbackAt(QmlListPropertyType* prop, int idx) {
        return static_cast<CppListType*>(prop->data)->at(idx);
    }

private:
    CppListType m_items;
};

#define LIST_PROPERTY(TYPE, NAME)                                                       \
 private:                                                                               \
  Q_PROPERTY(QQmlListProperty<TYPE> NAME READ MAKE_GETTER_NAME(NAME) CONSTANT)          \
 public:                                                                                \
  const QQmlSmartListWrapper<TYPE>& MAKE_GETTER_NAME(NAME)() const { return m_##NAME; } \
                                                                                        \
 private:                                                                               \
  QQmlSmartListWrapper<TYPE> m_##NAME;