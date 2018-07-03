#ifndef __TUBE_H
#define __TUBE_H

#include "hmi.h"

// ���������� �������� ��������
#define TUBE_NIXIE_COUNT        HMI_RANK_COUNT
// ���������� �������� ������������
#define TUBE_NEON_COUNT         4

// ����� ��� �������
#define TUBE_NIXIE_DIGIT_SPACE  10

// ������������� ������
void tube_init(void);
// ���������� ��������� ����
void tube_refresh(void);

// --- ����� --- //

// ��������� ����� (� ������)
void tube_nixie_digit_set(hmi_rank_t index, uint8_t digit, bool dot);
// ��������� ������
void tube_nixie_text_set(const char text[]); // TODO: � ���� �� ���?
// ��������� ������������
void tube_nixie_sat_set(hmi_sat_t value); // TODO: � ���� �� ���?
void tube_nixie_sat_set(hmi_rank_t index, hmi_sat_t value);

// --- ������ --- //

// ��������� ������������
void tube_neon_sat_set(hmi_sat_t value); // TODO: � ���� �� ���?
void tube_neon_sat_set(hmi_rank_t index, hmi_sat_t value);

// ���������� ������ ���������� ����
void tube_interrupt_nixie_selcrst(void);

#endif // __TUBE_H
