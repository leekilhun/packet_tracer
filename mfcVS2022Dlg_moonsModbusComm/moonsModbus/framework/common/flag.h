#pragma once

//#include "pch.h"

/// <summary>
/// 비트, 바이트 단위 플래그 데이터 형
/// </summary>
/// 
const int DEF_FLAG_BANK_MAX = 255;
enum class bit_e {
  b0,
  b1,
  b2,
  b3,
  b4,
  b5,
  b6,
  b7,
  _max
};
#define REG_BIT(x)          static_cast<bit_e>(x)

class GFlagData {

  struct _flag
  {
    bool m_bit[static_cast<uint8_t>(bit_e::_max)]{ 0 };
    explicit _flag(const _flag& ref) :m_bit() {}
    _flag& operator=(const _flag& ref) { return *this; }
  public:
    explicit _flag() :m_bit() {};
    virtual ~_flag() {};

    inline bool& operator[](const bit_e bit_no) {
      return m_bit[static_cast<int>(bit_no)];
    }

    inline void operator = (const uint8_t in_data) {
      uint8_t data = 0;
      for (uint8_t i = 0; i < 8; i++) {
        data = in_data;
        data = data >> i;
        data = 0xfe | data;
        data = ~(data);
        m_bit[i] = !data; //!(~(0xfe | (data >> i)));
      }
    }

    inline uint8_t get() {
      uint8_t data;
      data = (int)m_bit[0] | ((int)m_bit[1] * 2) | ((int)m_bit[2] * 4) | ((int)m_bit[3] * 8)
        | ((int)m_bit[4] * 16) | ((int)m_bit[5] * 32) | ((int)m_bit[6] * 64 | ((int)m_bit[7] * 128));
      return data;
    }

    inline uint8_t on_bit(const bit_e bit_no) {
      m_bit[static_cast<int>(bit_no)] = true;
      return get();
    }

    inline uint8_t off_bit(const bit_e bit_no) {
      m_bit[static_cast<int>(bit_no)] = false;
      return get();
    }
  };

public:
  _flag m_byte[DEF_FLAG_BANK_MAX];
  uint8_t m_size;

  explicit GFlagData(const GFlagData& ref) :m_byte(), m_size() {}
  GFlagData& operator=(const GFlagData ref) { return *this; }
public:
  GFlagData(uint8_t size = DEF_FLAG_BANK_MAX) :m_byte(), m_size(size) { };
  virtual ~GFlagData() {};

public:
  inline void ClearAll() {
    for (uint8_t i = 0; i < m_size; i++) {
      Clear(i);
    }
  }

  inline void Clear(uint8_t bank) {
    m_byte[bank] = 0;
  }

  inline uint8_t SetOn(uint8_t bank, bit_e bit_no) {
    m_byte[bank][bit_no] = true;
    return m_byte[bank].get();
  }
  inline uint8_t SetOff(uint8_t bank, bit_e bit_no) {
    m_byte[bank][bit_no] = false;
    return m_byte[bank].get();
  }

  inline bool Get(uint8_t bank, bit_e bit) {
    return m_byte[bank][bit];
  }

  inline uint8_t GetSize() const {
    return m_size;
  }

  inline _flag& operator[](uint8_t bank) {
    return m_byte[bank];
  }

  inline uint8_t operator[](int bit) {
    return m_byte[bit].get();
  }

};

