#include "tlvcommand.h"

TLVcommand::TLVcommand()
{
    m_type = UNKN;
    m_color = Qt::white;
}

TLVcommand::~TLVcommand()
{

}

TLVcommand::pufCommand TLVcommand::type() const
{
    return static_cast<TLVcommand::pufCommand>(m_type);
}

void TLVcommand::setType(TLVcommand::pufCommand type)
{
    m_type = type;
}

int TLVcommand::lenght() const
{
    return m_lenght;
}

void TLVcommand::setLenght(int lenght)
{
    m_lenght = lenght;
}

char TLVcommand::value() const
{
    return m_value;
}

void TLVcommand::setValue(char value)
{
    m_value = value;
}

QColor TLVcommand::color() const
{
    return m_color;
}

void TLVcommand::setColor(QColor color)
{
    m_color = color;
}
