//------------------------------------------------------------------------------
// <copyright file="ImageProcesser.cpp" company="Wentworth Institute of Technology">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "User.h"

/// <summary>
/// Constructor
/// </summary>
CUser::CUser() :
	m_sFirstName("Kevin"),
	m_sLastName("Camidge"),
	m_sFullName(m_sFirstName + " " + m_sLastName)
{

}

/// <summary>
/// Destructor
/// </summary>
CUser::~CUser()
{
}
