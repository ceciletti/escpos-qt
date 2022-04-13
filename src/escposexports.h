/* 
 * SPDX-FileCopyrightText: (C) 2022 Daniel Nicoletti <dantti12@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef ESC_POS_QT_EXPORT_H
#define ESC_POS_QT_EXPORT_H

#include <QtCore/QtGlobal>

#if defined(EscPosQt5_EXPORTS) || defined(EscPosQt6_EXPORTS)
#define ESC_POS_QT_EXPORT Q_DECL_EXPORT
#else
#define ESC_POS_QT_EXPORT Q_DECL_IMPORT
#endif

#endif
