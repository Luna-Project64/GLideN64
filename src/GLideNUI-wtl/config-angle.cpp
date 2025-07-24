#include "config-angle.h"
#include "UIConfig.h"
#include "Language.h"
#include "resource.h"

CAngleTab::CAngleTab() :
	CConfigTab(IDD_TAB_ANGLE)
{
}

BOOL CAngleTab::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/) {
	return true;
}

void CAngleTab::ApplyLanguage(void) {
}

LRESULT CAngleTab::OnColorStatic(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	return (LRESULT)GetStockObject(WHITE_BRUSH);
}

void CAngleTab::LoadSettings(bool /*blockCustomSettings*/) {
	CButton(GetDlgItem(IDC_RADIO_RENDER_AUTOMATIC)).SetCheck(config.angle.renderer == Config::arAdaptive ? BST_CHECKED : BST_UNCHECKED);
	CButton(GetDlgItem(IDC_RADIO_VULKAN)).SetCheck(config.angle.renderer == Config::arVulkan ? BST_CHECKED : BST_UNCHECKED);
	CButton(GetDlgItem(IDC_RADIO_OPENGL)).SetCheck(config.angle.renderer == Config::arOpenGL ? BST_CHECKED : BST_UNCHECKED);
	CButton(GetDlgItem(IDC_RADIO_DIRECT3D)).SetCheck(config.angle.renderer == Config::arDirectX11 ? BST_CHECKED : BST_UNCHECKED);
	CButton(GetDlgItem(IDC_CHECK_DIRECT_COMPOSITION)).SetCheck((config.angle.directComposition) != 0 ? BST_CHECKED : BST_UNCHECKED);
	CButton(GetDlgItem(IDC_RADIO_DEPTH_WRITE_ADAPTIVE)).SetCheck(config.generalEmulation.enableFragmentDepthWrite == Config::FragDepthWriteMode::adaptive ? BST_CHECKED : BST_UNCHECKED);
	CButton(GetDlgItem(IDC_RADIO_DEPTH_WRITE_ENABLED)).SetCheck(config.generalEmulation.enableFragmentDepthWrite == Config::FragDepthWriteMode::enabled ? BST_CHECKED : BST_UNCHECKED);
	CButton(GetDlgItem(IDC_RADIO_DEPTH_WRITE_DISABLED)).SetCheck(config.generalEmulation.enableFragmentDepthWrite == Config::FragDepthWriteMode::disabled ? BST_CHECKED : BST_UNCHECKED);
	CEdit(GetDlgItem(IDC_EDIT_SHADOW_DELIMITER)).SetWindowText(std::to_wstring(config.angle.shadowDelimiter).c_str());
}

void CAngleTab::SaveSettings() {
	if (CButton(GetDlgItem(IDC_RADIO_DIRECT3D)).GetCheck() == BST_CHECKED)
	{
		config.angle.renderer = Config::arDirectX11;
	}
	if (CButton(GetDlgItem(IDC_RADIO_VULKAN)).GetCheck() == BST_CHECKED)
	{
		config.angle.renderer = Config::arVulkan;
	}
	if (CButton(GetDlgItem(IDC_RADIO_OPENGL)).GetCheck() == BST_CHECKED)
	{
		config.angle.renderer = Config::arOpenGL;
	}
	config.angle.directComposition = CButton(GetDlgItem(IDC_CHECK_DIRECT_COMPOSITION)).GetCheck() == BST_CHECKED;
	if (CButton(GetDlgItem(IDC_RADIO_DEPTH_WRITE_ADAPTIVE)).GetCheck() == BST_CHECKED)
	{
		config.generalEmulation.enableFragmentDepthWrite = Config::FragDepthWriteMode::adaptive;
	}
	if (CButton(GetDlgItem(IDC_RADIO_DEPTH_WRITE_ENABLED)).GetCheck() == BST_CHECKED)
	{
		config.generalEmulation.enableFragmentDepthWrite = Config::FragDepthWriteMode::enabled;
	}
	if (CButton(GetDlgItem(IDC_RADIO_DEPTH_WRITE_DISABLED)).GetCheck() == BST_CHECKED)
	{
		config.generalEmulation.enableFragmentDepthWrite = Config::FragDepthWriteMode::disabled;
	}

	int maxLen = 5;
	std::wstring path;
	path.resize(maxLen + 1);
	CEdit(GetDlgItem(IDC_EDIT_SHADOW_DELIMITER)).GetWindowText((wchar_t*)path.data(), static_cast<int>(path.size()));

	config.angle.shadowDelimiter = std::stoi(path.c_str());
}
