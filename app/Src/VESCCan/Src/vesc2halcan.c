#include "vesc2halcan.h"

uint8_t vesc2halcan(FDCAN_TxHeaderTypeDef* txHeader, uint8_t* txData, size_t txDataSize, const VESC_RawFrame* vescRawFrame)
{
	if (txDataSize < vescRawFrame->can_dlc)
		return VESC2HALCAN_ERR_TXDATA;

	txHeader->Identifier = *(uint32_t*)vescRawFrame & 0x1FFFFFFF;
	txHeader->IdType = FDCAN_EXTENDED_ID;
	txHeader->TxFrameType = FDCAN_DATA_FRAME;

	// see FDCAN_data_length_code
	if (vescRawFrame->can_dlc >= 0u && vescRawFrame->can_dlc <= 8u)
		txHeader->DataLength = vescRawFrame->can_dlc;
	else if (vescRawFrame->can_dlc >= 12u && vescRawFrame->can_dlc <= 24u && vescRawFrame->can_dlc % 4u == 0u)
		txHeader->DataLength = 6u + (vescRawFrame->can_dlc / 4u);
	else if (vescRawFrame->can_dlc == 32u)
		txHeader->DataLength = FDCAN_DLC_BYTES_32;
	else if (vescRawFrame->can_dlc == 48u)
		txHeader->DataLength = FDCAN_DLC_BYTES_48;
	else if (vescRawFrame->can_dlc == 64u)
		txHeader->DataLength = FDCAN_DLC_BYTES_64;
	else
		return VESC2HALCAN_ERR_DLC;

	txHeader->ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	txHeader->BitRateSwitch = FDCAN_BRS_OFF;
	txHeader->FDFormat = FDCAN_CLASSIC_CAN;
	txHeader->TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	txHeader->MessageMarker = 0x0;

	memcpy(txData, vescRawFrame->rawData, vescRawFrame->can_dlc);

	return VESC2HALCAN_OK;
}

uint8_t halcan2vesc(VESC_RawFrame* vescRawFrame, const FDCAN_RxHeaderTypeDef* rxHeader, const uint8_t* rxData)
{
	uint8_t realDlc = 0;
	switch (rxHeader->DataLength)
	{
		case FDCAN_DLC_BYTES_0:
		case FDCAN_DLC_BYTES_1:
		case FDCAN_DLC_BYTES_2:
		case FDCAN_DLC_BYTES_3:
		case FDCAN_DLC_BYTES_4:
		case FDCAN_DLC_BYTES_5:
		case FDCAN_DLC_BYTES_6:
		case FDCAN_DLC_BYTES_7:
		case FDCAN_DLC_BYTES_8:
			realDlc = (uint8_t)rxHeader->DataLength;
			break;
		case FDCAN_DLC_BYTES_12:
			realDlc = 12;
			break;
		case FDCAN_DLC_BYTES_16:
			realDlc = 16;
			break;
		case FDCAN_DLC_BYTES_20:
			realDlc = 20;
			break;
		case FDCAN_DLC_BYTES_24:
			realDlc = 24;
			break;
		case FDCAN_DLC_BYTES_32:
			realDlc = 32;
			break;
		case FDCAN_DLC_BYTES_48:
			realDlc = 48;
			break;
		case FDCAN_DLC_BYTES_64:
			realDlc = 64;
			break;
	}
	
	if (VESC_CAN_DLEN_MAX < realDlc) return VESC2HALCAN_ERR_RXDATA;

	*(uint32_t*)vescRawFrame = rxHeader->Identifier;
	vescRawFrame->_reserved = VESC_CAN_EXTID_FLAG;
	vescRawFrame->can_dlc = realDlc;

	memcpy(vescRawFrame->rawData, rxData, realDlc);

	return VESC2HALCAN_OK;
}
