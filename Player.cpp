#include "Player.h"

OGLWidget *Player::w = nullptr;
QLabel *Player::imgLable = nullptr;

Player::Player(QObject *parent)
	: QThread(parent)
{
	w = new OGLWidget;
	w->show();

	m_vlcParam.oglWgt = w;
}

Player::~Player()
{
}

void *Player::vlc_lock(void *op, void **plane)
{
	auto p = static_cast<VideoCallbackParam *>(op);
	p->mutex.lock();
	*plane = p->pixals;
	return nullptr;
}

void Player::vlc_unlock(void *op, void *pic, void *const *plane)
{
	auto p = static_cast<VideoCallbackParam *>(op);
	p->mutex.unlock();
}

void Player::vlc_display(void *op, void *pic)
{
	auto p = static_cast<VideoCallbackParam *>(op);

	if (p->oglWgt)
	{
		p->oglWgt->setTexture(p->pixals, p->px, p->py);
	}
}

unsigned Player::formatCallback(void **opaque, char *chroma,
	unsigned * width, unsigned * height,
	unsigned * pitches,
	unsigned * lines)
{
	const auto x = *width;
	const auto y = *height;
	chroma[0] = 'R';
	chroma[1] = 'V';
	chroma[2] = '2';
	chroma[3] = '4';
	
	*pitches = x * 3;
	*lines = y;
	const auto p = static_cast<VideoCallbackParam *>(*opaque);
	p->pixals = new unsigned char[x * y * 3];
	memset(p->pixals, 0, x * y * 3);
	p->px = x;
	p->py = y;
	/*
	chroma[0] = 'R';
	chroma[1] = 'G';
	chroma[2] = 'B';
	chroma[3] = 'A';

	*pitches = x * 4;
	*lines = y;
	const auto p = static_cast<VideoCallbackParam *>(*opaque);
	p->pixals = new unsigned char[x * y * 4];
	memset(p->pixals, 0, x * y * 4);
	p->px = x;
	p->py = y;*/

	return 1;
}

void Player::clearUpCallback(void *opaque)
{
	const auto p = static_cast<VideoCallbackParam *>(opaque);

	delete[] p->pixals;
	p->pixals = nullptr;
}

void Player::play()
{
	libvlc_instance_t * inst = 0;
	libvlc_media_player_t *mp = 0;
	libvlc_media_t *m = 0;

	inst = libvlc_new(0, NULL);
	m = libvlc_media_new_path(inst, "test.mp4");
	mp = libvlc_media_player_new_from_media(m);

	//
	libvlc_video_set_callbacks(mp, vlc_lock, vlc_unlock, vlc_display, &m_vlcParam);
	libvlc_video_set_format_callbacks(mp, formatCallback, clearUpCallback);

	libvlc_media_release(m);
	libvlc_media_player_play(mp);

	return;

	libvlc_media_player_stop(mp);
	libvlc_media_player_release(mp);

	libvlc_release(inst);
}
