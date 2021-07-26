// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� FINESIGNIN_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ���Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// FINESIGNIN_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef  FINESIGNIN_EXPORTS
#define FINESIGNIN_API __declspec(dllexport)
#else
#define FINESIGNIN_API __declspec(dllimport)
#endif

#include <vector>
using signinnumlist = std::pair<std::string, long>;
using signinlxqdlist = std::pair<std::string, long>;
namespace finesignin {
	FINESIGNIN_API std::vector<signinnumlist> getsigninnumlist();
	FINESIGNIN_API std::vector<signinlxqdlist> getsigninlxqdlist();
};