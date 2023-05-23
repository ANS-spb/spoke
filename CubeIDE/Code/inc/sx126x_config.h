/*
	SPOKE
    
    file: sx126x_config.h
*/



#ifndef SX126X_CONFIG_HEADER
#define SX126X_CONFIG_HEADER


			//Size (Command + Arguments), Command, Argument1, Argument2, ...
#define 	SX126X_CONFIG_ARRAY {\
			SX126X_SIZE_CLR_DEVICE_ERRORS, SX126X_CLR_DEVICE_ERRORS, SX126X_NOP, SX126X_NOP, \
			SX126X_SIZE_SET_STANDBY, SX126X_SET_STANDBY, STDBY_RC, \
			SX126X_SIZE_SET_DIO2_AS_RF_SWITCH_CTRL, SX126X_SET_DIO2_AS_RF_SWITCH_CTRL, DIO2_AS_RF_ENABLED, \
			SX126X_SIZE_SET_DIO3_AS_TCXO_CTRL, SX126X_SET_DIO3_AS_TCXO_CTRL, TCXO_CTRL_1_8V, TCXO_DELAY_5MS_23_16, TCXO_DELAY_5MS_15_8, TCXO_DELAY_5MS_7_0, \
			SX126X_SIZE_SET_REGULATOR_MODE, SX126X_SET_REGULATOR_MODE, USE_DCDC, \
			SX126X_SIZE_CALIBRATE, SX126X_CALIBRATE, CALIBRATE_ALL, \
			SX126X_SIZE_CALIBRATE_IMAGE, SX126X_CALIBRATE_IMAGE, CALIBRATE_IMAGE_FREQ1_430M, CALIBRATE_IMAGE_FREQ2_440M, \
			SX126X_SIZE_SET_PKT_TYPE, SX126X_SET_PKT_TYPE, PACKET_TYPE_GFSK, \
			SX126X_SIZE_SET_RF_FREQUENCY, SX126X_SET_RF_FREQUENCY, RF_FREQ_LPD_CH1_31_24, RF_FREQ_LPD_CH1_23_16, RF_FREQ_LPD_CH1_15_8, RF_FREQ_LPD_CH1_7_0, \
			SX126X_SIZE_SET_PA_CFG, SX126X_SET_PA_CFG, PA_CFG_DUTY_CYCLE_22DB, PA_CFG_HP_MAX_22DB, PA_CFG_DEV_SEL, PA_CFG_LUT, \
			SX126X_SIZE_SET_TX_PARAMS, SX126X_SET_TX_PARAMS, TX_POWER_NEG9DB, TX_RAMP_TIME_800U, \
			SX126X_SIZE_SET_BUFFER_BASE_ADDRESS, SX126X_SET_BUFFER_BASE_ADDRESS, BASE_ADDR_TX, BASE_ADDR_RX, \
			SX126X_SIZE_SET_MODULATION_PARAMS_GFSK, SX126X_SET_MODULATION_PARAMS, FSK_MP1_BR_1200, FSK_MP2_BR_1200, FSK_MP3_BR_1200, FSK_MP4_SHAPE_00, FSK_MP5_RX_BW_11700, FSK_MP6_FDEV_1200, FSK_MP7_FDEV_1200, FSK_MP8_FDEV_1200, \
			SX126X_SIZE_SET_PKT_PARAMS_GFSK, SX126X_SET_PKT_PARAMS, FSK_PP1_PREAMB_3_BYTE, FSK_PP2_PREAMB_3_BYTE, FSK_PP3_PREAMB_THS_1_BYTE, FSK_PP4_SYNCW_LEN_2_BYTE, FSK_PP5_ADDR_COMP_DIS, FSK_PP6_PKT_TYPE_FIXED, FSK_PP7_PLOAD_LEN_12_BYTE, FSK_PP8_CRC_TYPE_2_BYTE, FSK_PP9_WHITE_NO, \
			SX126X_SIZE_WRITE_REGISTER + 2, SX126X_WRITE_REGISTER, SX126X_REG_ADDR_SYNC_WORD_0_1, SX126X_REG_ADDR_SYNC_WORD_0_0, SYNC_WORD_0, SYNC_WORD_1, \
			SX126X_SIZE_WRITE_REGISTER + 4, SX126X_WRITE_REGISTER, SX126X_REG_ADDR_CRC_INIT_VAL_1, SX126X_REG_ADDR_CRC_INIT_VAL_0, CRC_IBM_SEED_1, CRC_IBM_SEED_0, CRC_POLYNOMIAL_IBM_1, CRC_POLYNOMIAL_IBM_0, \
			SX126X_SIZE_SET_DIO_IRQ_PARAMS, SX126X_SET_DIO_IRQ_PARAMS, IRQ_MASK_1, IRQ_MASK_0, IRQ_DIO1_MASK_1, IRQ_DIO1_MASK_0, IRQ_DIO2_MASK_1, IRQ_DIO2_MASK_0, IRQ_DIO3_MASK_1, IRQ_DIO3_MASK_0, \
			SX126X_SIZE_SET_RX_TX_FALLBACK_MODE, SX126X_SET_RX_TX_FALLBACK_MODE, FALLBACK_MODE_XOSC, /* todo: set RC osc for lower consumption and check if 5 ms TCXO time is sufficient */\
			SX126X_SIZE_SET_STANDBY, SX126X_SET_STANDBY, STDBY_XOSC, \
			0x00  			/* end of the sequence */ \
			}



#endif /*SX126X_CONFIG_HEADER*/
