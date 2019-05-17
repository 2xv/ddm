//#include <iostream>
//#include <sys/time.h>

//#include <QCoreApplication>
//#include <QVector>

//#include "DDManager.h"

//using namespace std;

//int main (int argc, char *argv[])
//{
//    timeval tv;
//    gettimeofday (&tv, nullptr);
//    printf ("start %ld.%ld\n", tv.tv_sec, tv.tv_usec);

//    QCoreApplication a (argc, argv);

//    try
//    {
//        char                 byteDef     = 'a';
//        short                shortDef    =  12345;
//        int                  intDef      =  98765;
//        qint64               longDef     =  9876543210;
//        float                floatDef    =  1;
//        double               doubleDef   =  2;
//        qint64               numberDef   =  12;
//        qint64               snumberDef  = -34;
//        int                  timeDef     =  175112034;
//        int                  dateDef     =  20190427;
//        qint64               datetimeDef =  20190427175112034;
//        QByteArray           bytesDef     ("ProvaBytes");
//        QString              stringDef    ("ProvaString");
//        QVector <char>       vbyteDef     {'b','c'};
//        QVector <short>      vshortDef    {1,2,3};
//        QVector <int>        vintDef      {4,5,6};
//        QVector <qint64>     vlongDef     {7,8,9};
//        QVector <float>      vfloatDef    {10,11,12};
//        QVector <double>     vdoubleDef   {13,14,15};
//        QVector <qint64>     vnumberDef   {16,17,18};
//        QVector <qint64>     vsnumberDef  {-19,20,-21};
//        QVector <int>        vtimeDef     {timeDef};
//        QVector <int>        vdateDef     {dateDef};
//        QVector <qint64>     vdatetimeDef {datetimeDef};
//        QVector <QByteArray> vbytesDef    {bytesDef};
//        QVector <QString>    vstringDef   {stringDef};
//        DDList               listDef;

//        QVector <DDProps> props = {
//            {"byte",        0, 0, 0, 0, DDC_BYTE,       &byteDef,      nullptr},
//            {"short",       1, 0, 0, 0, DDC_SHORT,      &shortDef,     nullptr},
//            {"int",         2, 0, 0, 0, DDC_INT,        &intDef,       nullptr},
//            {"long",        3, 0, 0, 0, DDC_LONG,       &longDef,      nullptr},
//            {"float",       4, 0, 0, 0, DDC_FLOAT,      &floatDef,     nullptr},
//            {"double",      5, 0, 0, 0, DDC_DOUBLE,     &doubleDef,    nullptr},
//            {"number",      6, 0, 0, 0, DDC_NUMBER,     &numberDef,    nullptr},
//            {"snumber",     7, 0, 0, 0, DDC_S_NUMBER,   &snumberDef,   nullptr},
//            {"time",        8, 0, 0, 0, DDC_TIME,       &timeDef,      nullptr},
//            {"date",        9, 0, 0, 0, DDC_DATE,       &dateDef,      nullptr},
//            {"datetime",   10, 0, 0, 0, DDC_DATETIME,   &datetimeDef,  nullptr},
//            {"bytes",      11, 0, 0, 0, DDC_BYTES,      &bytesDef,     nullptr},
//            {"string",     12, 0, 0, 0, DDC_STRING,     &stringDef,    nullptr},
//            {"a_byte",     13, 0, 0, 0, DDC_A_BYTE,     &vbyteDef,     nullptr},
//            {"a_short",    14, 0, 0, 0, DDC_A_SHORT,    &vshortDef,    nullptr},
//            {"a_int",      15, 0, 0, 0, DDC_A_INT,      &vintDef,      nullptr},
//            {"a_long",     16, 0, 0, 0, DDC_A_LONG,     &vlongDef,     nullptr},
//            {"a_float",    17, 0, 0, 0, DDC_A_FLOAT,    &vfloatDef,    nullptr},
//            {"a_double",   18, 0, 0, 0, DDC_A_DOUBLE,   &vdoubleDef,   nullptr},
//            {"a_number",   19, 0, 0, 0, DDC_A_NUMBER,   &vnumberDef,   nullptr},
//            {"a_snumber",  20, 0, 0, 0, DDC_A_S_NUMBER, &vsnumberDef,  nullptr},
//            {"a_time",     21, 0, 0, 0, DDC_A_TIME,     &vtimeDef,     nullptr},
//            {"a_date",     22, 0, 0, 0, DDC_A_DATE,     &vdateDef,     nullptr},
//            {"a_datetime", 23, 0, 0, 0, DDC_A_DATETIME, &vdatetimeDef, nullptr},
//            {"a_bytes",    24, 0, 0, 0, DDC_A_BYTES,    &vbytesDef,    nullptr},
//            {"a_string",   25, 0, 0, 0, DDC_A_STRING,   &vstringDef,   nullptr},
//            {"list  ",     26, 0, 0, 0, DDC_LIST,       &listDef,      new QVector <DDProps>
//                {{"byte",       4096, 0, 0, 0, DDC_BYTE,       &byteDef,      nullptr},
//                 {"short",      4097, 0, 0, 0, DDC_SHORT,      &shortDef,     nullptr},
//                 {"int",        4098, 0, 0, 0, DDC_INT,        &intDef,       nullptr},
//                 {"long",       4099, 0, 0, 0, DDC_LONG,       &longDef,      nullptr},
//                 {"float",      4100, 0, 0, 0, DDC_FLOAT,      &floatDef,     nullptr},
//                 {"double",     4101, 0, 0, 0, DDC_DOUBLE,     &doubleDef,    nullptr},
//                 {"number",     4102, 0, 0, 0, DDC_NUMBER,     &numberDef,    nullptr},
//                 {"snumber",    4103, 0, 0, 0, DDC_S_NUMBER,   &snumberDef,   nullptr},
//                 {"time",       4104, 0, 0, 0, DDC_TIME,       &timeDef,      nullptr},
//                 {"date",       4105, 0, 0, 0, DDC_DATE,       &dateDef,      nullptr},
//                 {"datetime",   4106, 0, 0, 0, DDC_DATETIME,   &datetimeDef,  nullptr},
//                 {"bytes",      4107, 0, 0, 0, DDC_BYTES,      &bytesDef,     nullptr},
//                 {"string",     4108, 0, 0, 0, DDC_STRING,     &stringDef,    nullptr},
//                 {"a_byte",     4109, 0, 0, 0, DDC_A_BYTE,     &vbyteDef,     nullptr},
//                 {"a_short",    4110, 0, 0, 0, DDC_A_SHORT,    &vshortDef,    nullptr},
//                 {"a_int",      4111, 0, 0, 0, DDC_A_INT,      &vintDef,      nullptr},
//                 {"a_long",     4112, 0, 0, 0, DDC_A_LONG,     &vlongDef,     nullptr},
//                 {"a_float",    4113, 0, 0, 0, DDC_A_FLOAT,    &vfloatDef,    nullptr},
//                 {"a_double",   4114, 0, 0, 0, DDC_A_DOUBLE,   &vdoubleDef,   nullptr},
//                 {"a_number",   4115, 0, 0, 0, DDC_A_NUMBER,   &vnumberDef,   nullptr},
//                 {"a_snumber",  4116, 0, 0, 0, DDC_A_S_NUMBER, &vsnumberDef,  nullptr},
//                 {"a_time",     4117, 0, 0, 0, DDC_A_TIME,     &vtimeDef,     nullptr},
//                 {"a_date",     4118, 0, 0, 0, DDC_A_DATE,     &vdateDef,     nullptr},
//                 {"a_datetime", 4119, 0, 0, 0, DDC_A_DATETIME, &vdatetimeDef, nullptr},
//                 {"a_bytes",    4120, 0, 0, 0, DDC_A_BYTES,    &vbytesDef,    nullptr},
//                 {"a_string",   4121, 0, 0, 0, DDC_A_STRING,   &vstringDef,   nullptr}}
//            },
//        };

//        listDef.addByte              (byteDef);
//        listDef.addShort             (shortDef);
//        listDef.addInt               (intDef);
//        listDef.addLong              (longDef);
//        listDef.addFloat             (floatDef);
//        listDef.addDouble            (doubleDef);
//        listDef.addNumber            (numberDef);
//        listDef.addSignedNumber      (snumberDef);
//        listDef.addTime              (timeDef);
//        listDef.addDate              (dateDef);
//        listDef.addDateTime          (datetimeDef);
//        listDef.addBytes             (&bytesDef);
//        listDef.addString            (&stringDef);
//        listDef.addByteArray         (&vbyteDef);
//        listDef.addShortArray        (&vshortDef);
//        listDef.addIntArray          (&vintDef);
//        listDef.addLongArray         (&vlongDef);
//        listDef.addFloatArray        (&vfloatDef);
//        listDef.addDoubleArray       (&vdoubleDef);
//        listDef.addNumberArray       (&vnumberDef);
//        listDef.addSignedNumberArray (&vsnumberDef);
//        listDef.addTimeArray         (&vtimeDef);
//        listDef.addDateArray         (&vdateDef);
//        listDef.addDateTimeArray     (&vdatetimeDef);
//        listDef.addBytesArray        (&vbytesDef);
//        listDef.addStringArray       (&vstringDef);

//        DDManager ddm;
//        ddm.setProperties (props);

//        cout << props  [0].name << "\t" << ddm.getByte               (0) << endl;
//        cout << props  [1].name << "\t" << ddm.getShort              (1) << endl;
//        cout << props  [2].name << "\t" << ddm.getInt                (2) << endl;
//        cout << props  [3].name << "\t" << ddm.getLong               (3) << endl;
//        cout << props  [4].name << "\t" << ddm.getFloat              (4) << endl;
//        cout << props  [5].name << "\t" << ddm.getDouble             (5) << endl;
//        cout << props  [6].name << "\t" << ddm.getNumber             (6) << endl;
//        cout << props  [7].name << "\t" << ddm.getSignedNumber       (7) << endl;
//        cout << props  [8].name << "\t" << ddm.getTime               (8) << endl;
//        cout << props  [9].name << "\t" << ddm.getDate               (9) << endl;
//        cout << props [10].name << "\t" << ddm.getDateTime          (10) << endl;
//        cout << props [11].name << "\t" << ddm.getBytes             (11).toStdString () << endl;
//        cout << props [12].name << "\t" << ddm.getString            (12).toStdString () << endl;
//        cout << props [13].name << "\t" << ddm.getByteArray         (13).data () [0] << endl;
//        cout << props [14].name << "\t" << ddm.getShortArray        (14).data () [0] << endl;
//        cout << props [15].name << "\t" << ddm.getIntArray          (15).data () [0] << endl;
//        cout << props [16].name << "\t" << ddm.getLongArray         (16).data () [0] << endl;
//        cout << props [17].name << "\t" << ddm.getFloatArray        (17).data () [0] << endl;
//        cout << props [18].name << "\t" << ddm.getDoubleArray       (18).data () [0] << endl;
//        cout << props [19].name << "\t" << ddm.getNumberArray       (19).data () [0] << endl;
//        cout << props [20].name << "\t" << ddm.getSignedNumberArray (20).data () [0] << endl;
//        cout << props [21].name << "\t" << ddm.getTimeArray         (21).data () [0] << endl;
//        cout << props [22].name << "\t" << ddm.getDateArray         (22).data () [0] << endl;
//        cout << props [23].name << "\t" << ddm.getDateTimeArray     (23).data () [0] << endl;
//        cout << props [24].name << "\t" << ddm.getBytesArray        (24).data () [0].toStdString () << endl;
//        cout << props [25].name << "\t" << ddm.getStringArray       (25).data () [0].toStdString () << endl;
//        cout << props [26].name << "\t" << ddm.getList              (26).getDataNumber() << endl;

//        ddm.setByte               (0, byteDef);
//        ddm.setShort              (1, shortDef);
//        ddm.setInt                (2, intDef);
//        ddm.setLong               (3, longDef);
//        ddm.setFloat              (4, floatDef);
//        ddm.setDouble             (5, doubleDef);
//        ddm.setNumber             (6, numberDef);
//        ddm.setSignedNumber       (7, snumberDef);
//        ddm.setTime               (8, timeDef);
//        ddm.setDate               (9, dateDef);
//        ddm.setDateTime          (10, datetimeDef);
//        ddm.setBytes             (11, bytesDef);
//        ddm.setString            (12, stringDef);
//        ddm.setByteArray         (13, vbyteDef);
//        ddm.setShortArray        (14, vshortDef);
//        ddm.setIntArray          (15, vintDef);
//        ddm.setLongArray         (16, vlongDef);
//        ddm.setFloatArray        (17, vfloatDef);
//        ddm.setDoubleArray       (18, vdoubleDef);
//        ddm.setNumberArray       (19, vnumberDef);
//        ddm.setSignedNumberArray (20, vsnumberDef);
//        ddm.setTimeArray         (21, vtimeDef);
//        ddm.setDateArray         (22, vdateDef);
//        ddm.setDateTimeArray     (23, vdatetimeDef);
//        ddm.setBytesArray        (24, vbytesDef);
//        ddm.setStringArray       (25, vstringDef);
//        ddm.setList              (26, listDef);

//        cout << endl << ddm.toString ().toStdString () << endl;

//        gettimeofday (&tv, nullptr);
//        printf ("stop %ld.%ld\n", tv.tv_sec, tv.tv_usec);
//    }
//    catch (QString err) {cout << "DDManager: " << err.toStdString () << endl;}

//    return a.exec ();
//}