// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� LLHUNTER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ���Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// LLHUNTER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef  LLHUNTER_EXPORTS
#define LLHUNTER_API __declspec(dllexport)
#else
#define LLHUNTER_API __declspec(dllimport)
#endif

#include <vector>
using killmoblist = std::pair<std::string, long>;

namespace Hunter {
	LLHUNTER_API std::vector<killmoblist> getkillmoblist();
};