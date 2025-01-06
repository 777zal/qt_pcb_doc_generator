#ifndef BOMTOOLS_H
#define BOMTOOLS_H

#include <QObject>

class BomTools : public QObject
{
    Q_OBJECT
public:
    explicit BomTools(QObject *parent = nullptr);

signals:
};

#endif // BOMTOOLS_H
