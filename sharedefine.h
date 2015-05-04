#ifndef SHAREDEFINE_H
#define SHAREDEFINE_H

#define BroadcastPort 40000
#define PrivateChatPort 22222
#define FileSendPort 12345
#define SendFileDataPort 6666
#define VoiceChatPort 30011
#define keyString "587b14e9ef30e95b64dc5ec71230197a"  //32位字符密码
enum MessageType{Message,NewParticipant,ParticipantLeft,Shake,SENDFILE,VOICE};
#endif // SHAREDEFINE_H
