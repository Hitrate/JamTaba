
#include <QObject>
#include <QString>
#include <QtTest/QtTest>
#include "persistence/UsersDataCache.h"

using namespace Persistence;

class TestCacheEntry: public QObject
{
    Q_OBJECT

private slots:
    void validUserIp_data();
    void validUserIp();
    void invalidUserIp_data();
    void invalidUserIp();

    void validUserName_data();
    void validUserName();
    void invalidUserName_data();
    void invalidUserName();

    void defaultValues();

    void setPanGuard_data();
    void setPanGuard();
};


// NOTE: UsersDataCache class was not able to test without side effect
// because destructor write to storage directly.
class TestUsersDataCache: public QObject
{
    Q_OBJECT
private slots:
};


void TestCacheEntry::validUserIp_data()
{
    QTest::addColumn<QString>("addr");
    QTest::newRow("local address") << "127.0.0.1";
    QTest::newRow("local address with mask") << "127.0.0.x";

    // the regex pattern will allow following unusual data.

    // this is an invalid as IPv4 address, out of 8bit range
    // but we does not need to check. not making a connection.
    QTest::newRow("invalid but welformed") << "234.456.678.890";

    QTest::newRow("padding") << "000.000.000.x";
}

void TestCacheEntry::validUserIp()
{
    QFETCH(QString, addr);
    QVERIFY(CacheEntry::ipPattern.exactMatch(addr));
}

void TestCacheEntry::invalidUserIp_data()
{
    QTest::addColumn<QString>("addr");
    QTest::newRow("empty field") << "127.0..0";
    QTest::newRow("three fields") << "127.0.0";
    QTest::newRow("five fields") << "127.0.0.0.";
    QTest::newRow("non digits") << "x.x.x.x";
    QTest::newRow("negative number") << "-127.0.0.0";
}

void TestCacheEntry::invalidUserIp()
{
    QFETCH(QString, addr);
    QVERIFY(! CacheEntry::ipPattern.exactMatch(addr));
}

void TestCacheEntry::validUserName_data()
{
    QTest::addColumn<QString>("name");
    QTest::newRow("anon") << "anon";
    QTest::newRow("underscore") << "_";
}

void TestCacheEntry::validUserName()
{
    QFETCH(QString, name);
    QVERIFY(CacheEntry::namePattern.exactMatch(name));
}

void TestCacheEntry::invalidUserName_data()
{
    QTest::addColumn<QString>("name");
    QTest::newRow("empty name") << "";
    QTest::newRow("invalid letters") << "-";
}

void TestCacheEntry::invalidUserName()
{
    QFETCH(QString, name);
    QVERIFY(! CacheEntry::namePattern.exactMatch(name));
}

void TestCacheEntry::defaultValues()
{
    CacheEntry entry("0.0.0.x", "anon", 0);

    QCOMPARE(entry.getUserIP(), QStringLiteral("0.0.0.x"));
    QCOMPARE(entry.getUserName(), QStringLiteral("anon"));
    QCOMPARE(entry.getChannelID(), static_cast<quint8>(0));
    QCOMPARE(entry.isMuted(), false);
    QCOMPARE(entry.getGain(), 1.0f);
    QCOMPARE(entry.getPan(), 0.0f);
    QCOMPARE(entry.getBoost(), 1.0f);
}

void TestCacheEntry::setPanGuard_data()
{
    QTest::addColumn<float>("actual");
    QTest::addColumn<float>("expect");
    QTest::newRow("default") << 0.0f << 0.0f;
    QTest::newRow("min") << -4.0f << -4.0f;
    QTest::newRow("max") << 4.0f << 4.0f;
    QTest::newRow("out of min") << -4.1f << -4.0f;
    QTest::newRow("out of max") << 4.1f << 4.0f;
}

void TestCacheEntry::setPanGuard()
{
    QFETCH(float, actual);
    QFETCH(float, expect);
    CacheEntry entry("0.0.0.x", "anon", 0);

    entry.setPan(actual);
    QCOMPARE(entry.getPan(), expect);
}

int main(int argc, char *argv[])
{
    int status = 0;

    {
        TestCacheEntry test;
        status |= QTest::qExec(&test, argc, argv);
    }

    {
        TestUsersDataCache test;
        status |= QTest::qExec(&test, argc, argv);
    }

    return status;
}

#include "tst_UsersDataCache.moc"
