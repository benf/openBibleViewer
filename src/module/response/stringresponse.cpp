#include "stringresponse.h"

StringResponse::StringResponse(const QString &data) : m_data(data)
{
}

bool StringResponse::isValid() const
{
    return true;
}
Response::ResponseType StringResponse::type() const
{
    return Response::StringReponse;
}
QString StringResponse::data() const
{
    return m_data;
}
void StringResponse::append(const QString &t)
{
    m_data.append(t);
}

void StringResponse::prepend(const QString &t)
{
    m_data.prepend(t);
}