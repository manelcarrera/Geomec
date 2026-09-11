/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ModelMessage.h"

TModelMessage::TModelMessage(const QString &_text, const QString &_url) : text(_text), url(_url) {}

TModelError::TModelError(const QString &text, const QString &url, int _error)
    : TModelMessage(text, url), error(_error) {}

TModelError::TModelError(const QString &text, int _error) : TModelMessage(text), error(_error) {}
