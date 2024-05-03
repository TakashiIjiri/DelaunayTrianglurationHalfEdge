#pragma once


#pragma unmanaged
#include "OglForCLI.h"
#pragma managed


namespace DelaunayTriangulation {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// MainForm の概要
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	private:
		OglForCLI* m_ogl;
		MainForm(void);
		static MainForm^ m_singleton;

	public:
		static MainForm^ GetInst() // <--追加 シングルトンパターン
		{
			if (m_singleton == nullptr) m_singleton = gcnew MainForm();
			return m_singleton;
		}


		void MainForm::RedrawPanel();

	private: System::Windows::Forms::Panel^ m_panel;


	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
			this->m_panel = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// m_panel
			// 
			this->m_panel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_panel->Location = System::Drawing::Point(12, 12);
			this->m_panel->Name = L"m_panel";
			this->m_panel->Size = System::Drawing::Size(839, 632);
			this->m_panel->TabIndex = 0;
			this->m_panel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MainForm::m_panel_Paint);
			this->m_panel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::m_panel_MouseDown);
			this->m_panel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::m_panel_MouseMove);
			this->m_panel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::m_panel_MouseUp);
			this->m_panel->Resize += gcnew System::EventHandler(this, &MainForm::m_panel_Resize);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(863, 656);
			this->Controls->Add(this->m_panel);
			this->Name = L"MainForm";
			this->Text = L"MainForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void m_panel_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
	private: System::Void m_panel_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
	private: System::Void m_panel_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
	private: System::Void m_panel_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e);
	private: System::Void m_panel_Resize(System::Object^ sender, System::EventArgs^ e);
	};

	inline void MainFormRedraw(){MainForm::GetInst()->RedrawPanel(); }
}
