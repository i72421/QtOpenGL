#pragma once

#include "vlc/vlc.h"
#include <iostream>
#include <windows.h>
#include <QThread>
#include <QMutex>
#include <QLabel>

#include "OGLWidget.h"


using namespace std;

struct VideoCallbackParam
{
	QMutex mutex;
	unsigned char *pixals = nullptr;
	unsigned int px = 0;
	unsigned int py = 0;
	libvlc_media_player_t *mediaplayer = nullptr;
	OGLWidget *oglWgt = nullptr;
};

class Player : public QThread
{
	Q_OBJECT

public:
	Player(QObject *parent = nullptr);
	~Player();

	//static void VLCEvents(const libvlc_event_t* pEvt, void* pUserData);
	static void *vlc_lock(void *op, void **plane);
	static void vlc_unlock(void *op, void *pic, void *const *plane);
	static void vlc_display(void *op, void *pic);
	static unsigned formatCallback(void **opaque, char *chroma,
		unsigned * width, unsigned * height,
		unsigned * pitches,
		unsigned * lines);
	static void clearUpCallback(void *opaque);

	void play();

	static OGLWidget *w;
	static QLabel *imgLable;

	VideoCallbackParam m_vlcParam;
};
