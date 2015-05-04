#include "voiceio.h"

VoiceIO::VoiceIO(QObject *parent) :
    QObject(parent)
{
    isEnabled = false;

    QAudioFormat format;
    format.setSampleRate(8000); // 22050
    format.setSampleSize(8);    // 16
    format.setChannelCount(1);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format))
    {
        qWarning() << "Default format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
        qDebug() << format.sampleRate();
        qDebug() << format.sampleSize();
    }

    QAudioInput *InputAudio = new QAudioInput(format, this);//提供语音的录入和播出
    QAudioOutput *OutputAudio = new QAudioOutput(format, this);

    devInput  = InputAudio->start();
    devOutput = OutputAudio->start();

    connect(devInput, SIGNAL(readyRead()), this, SLOT(devInput_readyRead()));
}

void VoiceIO::writeVoice(QByteArray data)
{
    if(isEnabled) devOutput->write(data);
}

void VoiceIO::devInput_readyRead()
{
    QByteArray data = devInput->readAll();
    if(isEnabled) emit readVoice(data);
}

void VoiceIO::setEnabled(bool val)
{
    isEnabled = val;
}
