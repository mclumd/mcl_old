package mcl.jni;

class mclAPIconstants {
    protected mclAPIconstants() {}
    
    public static final int PCI_INTENTIONAL = 0;
    public static final int PCI_EFFECTORS_CAN_FAIL = 1;
    public static final int PCI_SENSORS_CAN_FAIL   = 2;
    public static final int PCI_PARAMETERIZED      = 3;
    public static final int PCI_DECLARATIVE        = 4;
    public static final int PCI_RETRAINABLE        = 5;
    public static final int PCI_HLC_CONTROLLING    = 6;
    public static final int PCI_HTN_IN_PLAY        = 7;
    public static final int PCI_PLAN_IN_PLAY       = 8;
    public static final int PCI_ACTION_IN_PLAY     = 9;
    public static final int PCI_MAX                = 10;

    public static final int CRC_IGNORE             = PCI_MAX;
    public static final int CRC_NOOP               = 11;
    public static final int CRC_TRY_AGAIN          = 12;
    public static final int CRC_SOLICIT_HELP       = 13;
    public static final int CRC_RELINQUISH_CONTROL = 14;
    public static final int CRC_SENSOR_DIAG        = 15;
    public static final int CRC_EFFECTOR_DIAG      = 16;
    public static final int CRC_ACTIVATE_LEARNING  = 17;
    public static final int CRC_ADJ_PARAMS         = 18;
    public static final int CRC_REBUILD_MODELS     = 19;
    public static final int CRC_REVISIT_ASSUMPTIONS = 20;
    public static final int CRC_AMEND_CONTROLLER   = 21;
    public static final int CRC_REVISE_EXPECTATIONS = 22;
    public static final int CRC_ALG_SWAP           = 23;
    public static final int CRC_CHANGE_HLC         = 24;
    public static final int PC_VECTOR_LENGTH       = 25;

    public static final int CRC_MAX = PC_VECTOR_LENGTH-PCI_MAX;

    public static final int PC_YES = 0xFFFFFFFF;
    public static final int PC_NO  = 0x00000000;

    public static final int EC_ILLEGAL = 0x00;

    public static final int EC_STAYUNDER     = 0x01;
    public static final int EC_STAYOVER      = 0x02;
    public static final int EC_MAINTAINVALUE = 0x04;
    public static final int EC_WITHINNORMAL  = 0x08;

    public static final int EC_REALTIME   = 0x10;
    public static final int EC_TICKTIME   = 0x11;

    public static final int EC_GO_UP      = 0x20;
    public static final int EC_GO_DOWN    = 0x21;
    public static final int EC_NET_ZERO   = 0x22;
    public static final int EC_ANY_CHANGE = 0x23;
    public static final int EC_NET_RANGE  = 0x24;
    public static final int EC_TAKE_VALUE = 0x30;

    public static final int DT_INTEGER  = 0x00;
    public static final int DT_RATIONAL = 0x01;
    public static final int DT_BINARY   = 0x02;
    public static final int DT_BITFIELD = 0x03;
    public static final int DT_SYMBOL   = 0x04;

    public static final int SC_STATE      = 0;
    public static final int SC_CONTROL    = 1;
    public static final int SC_SPATIAL    = 2;
    public static final int SC_TEMPORAL   = 3;
    public static final int SC_RESOURCE   = 4;
    public static final int SC_REWARD     = 5;
    public static final int SC_AMBIENT    = 6;
    public static final int SC_OBJECTPROP = 7; 
    public static final int SC_MESSAGE    = 8;
    public static final int SC_UNSPEC     = 9;

    public static final int SC_NUMCODES_LEGAL = SC_UNSPEC+1;

    public static final int PROP_DT     = 0;
    public static final int PROP_SCLASS = 1;
    public static final int PROP_NUMBER_OF_SENSOR_PROPS = 2;

}
