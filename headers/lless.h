// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� LLHUNTER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ���Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// LLHUNTER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef  LLESS_EXPORTS
#define LLESS_API __declspec(dllexport)
#else
#define LLESS_API __declspec(dllimport)
#endif

#include <MAP>

LLESS_API map<string, int> getServerIPlist();
LLESS_API map<string, string> getServerIPPlayerList();