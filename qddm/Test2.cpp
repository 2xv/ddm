//#include <iostream>
//#include <sys/time.h>

//#include <QCoreApplication>
//#include <QDate>
//#include <QDateTime>
//#include <QTime>
//#include <QVector>

//#include "DDManager.h"

//using namespace std;

//int main (int argc, char *argv[])
//{
//    QCoreApplication a (argc, argv);

//    timeval tv;
//    gettimeofday (&tv, nullptr);
//    printf ("start %ld.%ld\n", tv.tv_sec, tv.tv_usec);

//    try
//    {
//        QVector <DDProps> props =
//        {
//            {"transazione",      0, 4, 4, 0, DDC_STRING   | DDC_CHKLIM | DDC_REQ, nullptr,        nullptr},
//            {"ricevitoria",      1, 6, 6, 0, DDC_STRING   | DDC_CHKLIM | DDC_REQ, nullptr,        nullptr},
//            {"terminale",        2,'1',0, 0, DDC_BYTE     | DDC_CHKMIN | DDC_REQ, nullptr,        nullptr},
//            {"tuid",             3, 0, 0, 0, DDC_DATETIME,                        nullptr,        nullptr},
//            {"flagFirma",        4, 0, 0, 0, DDC_BYTE     | DDC_CHKMIN,           new char (0), nullptr},
//            {"flagCompressione", 5, 0, 0, 0, DDC_BYTE     | DDC_CHKMIN,           new char (0), nullptr},
//            {"versione",         6, 1, 0, 0, DDC_NUMBER   | DDC_CHKMIN | DDC_REQ, nullptr,        nullptr},
//            {"release",          7, 1, 0, 0, DDC_NUMBER   | DDC_CHKMIN | DDC_REQ, nullptr,        nullptr},
//            {"body",            15, 0, 0, 0, DDC_BYTES,                           nullptr,        nullptr}
//        };

//        DDManager ddm;
//        ddm.setProperties (props);

//        QString   s1  = "LGG7";
//        QString   s2  = "BA0001";

//        ddm.setString   (0,  s1);
//        ddm.setString   (1,  s2);
//        ddm.setByte     (2, '1');
//        ddm.setDateTime (3,  20190430153209000);
//        ddm.setNumber   (6,  1);
//        ddm.setNumber   (7,  1);

//        cout << endl << ddm.toString ().toStdString () << endl;

//        QByteArray  data;
//        ddm.encode (data);

//        cout << endl << data.size () << " [";
//        for (int i = 0; i < data.size () ; i++) cout << static_cast <int> (data.at (i)) << ' ';
//        cout << ']' << endl;

//        DDManager ddm1;

//        ddm1.decode (data);
//        cout << endl << ddm1.toString ().toStdString () << endl;

//        ddm1.setProperties (props);
//        cout << endl << ddm1.toString ().toStdString () << endl;

//        gettimeofday (&tv, nullptr);
//        printf ("stop %ld.%ld\n", tv.tv_sec, tv.tv_usec);
//    }
//    catch (QString err) {cout << "DDManager: " << err.toStdString () << endl;}

//    return a.exec ();
//}
