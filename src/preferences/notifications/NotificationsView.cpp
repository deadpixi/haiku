/*
 * Copyright 2010, Haiku, Inc. All Rights Reserved.
 * Copyright 2009, Pier Luigi Fiorini.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Pier Luigi Fiorini, pierluigi.fiorini@gmail.com
 */

#include <Alert.h>
#include <Catalog.h>
#include <CheckBox.h>
#include <ColumnListView.h>
#include <ColumnTypes.h>
#include <Directory.h>
#include <FindDirectory.h>
#include <LayoutBuilder.h>
#include <Notification.h>
#include <Path.h>
#include <TextControl.h>
#include <Window.h>

#include <notification/Notifications.h>
#include <notification/NotificationReceived.h>

#include "NotificationsView.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "NotificationView"

const float kEdgePadding = 5.0;
const float kCLVTitlePadding = 8.0;

const int32 kApplicationSelected = '_ASL';
const int32 kNotificationSelected = '_NSL';

const int32 kCLVDeleteRow = 'av02';

// Applications column indexes
const int32 kAppIndex = 0;
const int32 kAppEnabledIndex = 1;

// Notifications column indexes
const int32 kTitleIndex = 0;
const int32 kDateIndex = 1;
const int32 kTypeIndex = 2;
const int32 kAllowIndex = 3;


NotificationsView::NotificationsView(SettingsHost* host)
	:
	SettingsPane("apps", host)
{
	BRect rect(0, 0, 100, 100);

	// Search application field
	fSearch = new BTextControl(B_TRANSLATE("Search:"), NULL,
		new BMessage(kSettingChanged));

	// Applications list
	fApplications = new BColumnListView(B_TRANSLATE("Applications"),
		0, B_FANCY_BORDER, true);
	fApplications->SetSelectionMode(B_SINGLE_SELECTION_LIST);

	fAppCol = new BStringColumn(B_TRANSLATE("Application"), 200,
		be_plain_font->StringWidth(B_TRANSLATE("Application")) +
		(kCLVTitlePadding * 2), rect.Width(), B_TRUNCATE_END, B_ALIGN_LEFT);
	fApplications->AddColumn(fAppCol, kAppIndex);

	fAppEnabledCol = new BStringColumn(B_TRANSLATE("Enabled"), 10,
		be_plain_font->StringWidth(B_TRANSLATE("Enabled")) +
		(kCLVTitlePadding * 2), rect.Width(), B_TRUNCATE_END, B_ALIGN_LEFT);
	fApplications->AddColumn(fAppEnabledCol, kAppEnabledIndex);

	// Notifications list
	fNotifications = new BColumnListView(B_TRANSLATE("Notifications"),
		0, B_FANCY_BORDER, true);
	fNotifications->SetSelectionMode(B_SINGLE_SELECTION_LIST);

	fTitleCol = new BStringColumn(B_TRANSLATE("Title"), 100,
		be_plain_font->StringWidth(B_TRANSLATE("Title")) +
		(kCLVTitlePadding * 2), rect.Width(), B_TRUNCATE_END, B_ALIGN_LEFT);
	fNotifications->AddColumn(fTitleCol, kTitleIndex);

	fDateCol = new BDateColumn(B_TRANSLATE("Last Received"), 100,
		be_plain_font->StringWidth(B_TRANSLATE("Last Received")) +
		(kCLVTitlePadding * 2), rect.Width(), B_ALIGN_LEFT);
	fNotifications->AddColumn(fDateCol, kDateIndex);

	fTypeCol = new BStringColumn(B_TRANSLATE("Type"), 100,
		be_plain_font->StringWidth(B_TRANSLATE("Type")) +
		(kCLVTitlePadding * 2), rect.Width(), B_TRUNCATE_END, B_ALIGN_LEFT);
	fNotifications->AddColumn(fTypeCol, kTypeIndex);

	fAllowCol = new BStringColumn(B_TRANSLATE("Allowed"), 100,
		be_plain_font->StringWidth(B_TRANSLATE("Allowed")) +
		(kCLVTitlePadding * 2), rect.Width(), B_TRUNCATE_END, B_ALIGN_LEFT);
	fNotifications->AddColumn(fAllowCol, kAllowIndex);

	// Add views
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.AddGroup(B_HORIZONTAL)
			.AddGlue()
			.Add(fSearch)
		.End()
		.Add(fApplications)
		.Add(fNotifications)
		.SetInsets(B_USE_WINDOW_SPACING, B_USE_WINDOW_SPACING,
			B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING);
}


void
NotificationsView::AttachedToWindow()
{
	fApplications->SetTarget(this);
	fApplications->SetInvocationMessage(new BMessage(kApplicationSelected));

	fNotifications->SetTarget(this);
	fNotifications->SetInvocationMessage(new BMessage(kNotificationSelected));

#if 0
	fNotifications->AddFilter(new BMessageFilter(B_ANY_DELIVERY,
		B_ANY_SOURCE, B_KEY_DOWN, CatchDelete));
#endif
}


void
NotificationsView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case kApplicationSelected:
		{
			BRow* row = fApplications->CurrentSelection();
			if (row == NULL)
				return;
			BStringField* appName
				= dynamic_cast<BStringField*>(row->GetField(kAppIndex));
			if (appName == NULL)
				break;

			appusage_t::iterator it = fAppFilters.find(appName->String());
			if (it != fAppFilters.end())
				_Populate(it->second);

			break;
		}
		case kNotificationSelected:
			break;
		default:
			BView::MessageReceived(msg);
			break;
	}
}


status_t
NotificationsView::Load(BMessage& settings)
{
	type_code type;
	int32 count = 0;

	if (settings.GetInfo("app_usage", &type, &count) != B_OK)
		return B_ERROR;

	// Clean filters
	appusage_t::iterator auIt;
	for (auIt = fAppFilters.begin(); auIt != fAppFilters.end(); auIt++)
		delete auIt->second;
	fAppFilters.clear();

	// Add new filters
	for (int32 i = 0; i < count; i++) {
		AppUsage* app = new AppUsage();
		settings.FindFlat("app_usage", i, app);
		fAppFilters[app->Name()] = app;
	}

	// Load the applications list
	_PopulateApplications();

	return B_OK;
}


status_t
NotificationsView::Save(BMessage& storage)
{
	appusage_t::iterator fIt;
	for (fIt = fAppFilters.begin(); fIt != fAppFilters.end(); fIt++)
		storage.AddFlat("app_usage", fIt->second);

	return B_OK;
}


void
NotificationsView::_PopulateApplications()
{
	appusage_t::iterator it;

	fApplications->Clear();

	for (it = fAppFilters.begin(); it != fAppFilters.end(); ++it) {
		BRow* row = new BRow();
		row->SetField(new BStringField(it->first.String()), kAppIndex);
		fApplications->AddRow(row);
	}
}


void
NotificationsView::_Populate(AppUsage* usage)
{
	// Sanity check
	if (!usage)
		return;

	int32 size = usage->Notifications();

	if (usage->Allowed() == false)
		fBlockAll->SetValue(B_CONTROL_ON);

	fNotifications->Clear();

	for (int32 i = 0; i < size; i++) {
		NotificationReceived* notification = usage->NotificationAt(i);
		time_t updated = notification->LastReceived();
		const char* allow = notification->Allowed() ? B_TRANSLATE("Yes")
			: B_TRANSLATE("No");
		const char* type = "";

		switch (notification->Type()) {
			case B_INFORMATION_NOTIFICATION:
				type = B_TRANSLATE("Information");
				break;
			case B_IMPORTANT_NOTIFICATION:
				type = B_TRANSLATE("Important");
				break;
			case B_ERROR_NOTIFICATION:
				type = B_TRANSLATE("Error");
				break;
			case B_PROGRESS_NOTIFICATION:
				type = B_TRANSLATE("Progress");
				break;
			default:
				type = B_TRANSLATE("Unknown");
		}

		BRow* row = new BRow();
		row->SetField(new BStringField(notification->Title()), kTitleIndex);
		row->SetField(new BDateField(&updated), kDateIndex);
		row->SetField(new BStringField(type), kTypeIndex);
		row->SetField(new BStringField(allow), kAllowIndex);

		fNotifications->AddRow(row);
	}
}
