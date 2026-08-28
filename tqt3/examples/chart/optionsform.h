#ifndef OPTIONSFORM_H
#define OPTIONSFORM_H

#include <ntqdialog.h>

class TQButtonGroup;
class TQComboBox;
class TQFrame;
class TQGridLayout;
class TQHBoxLayout;
class TQLabel;
class TQPushButton;
class TQRadioButton;
class TQSpinBox;
class TQVBoxLayout;


class OptionsForm : public TQDialog
{
    TQ_OBJECT
public:
    OptionsForm( TQWidget* parent = 0, const char* name = "options form",
		 bool modal = false, WFlags f = 0 );
    ~OptionsForm() {}

    TQFont font() const { return m_font; }
    void setFont( TQFont font );

    TQLabel *chartTypeTextLabel;
    TQComboBox *chartTypeComboBox;
    TQPushButton *fontPushButton;
    TQLabel *fontTextLabel;
    TQFrame *addValuesFrame;
    TQButtonGroup *addValuesButtonGroup;
    TQRadioButton *noRadioButton;
    TQRadioButton *yesRadioButton;
    TQRadioButton *asPercentageRadioButton;
    TQLabel *decimalPlacesTextLabel;
    TQSpinBox *decimalPlacesSpinBox;
    TQPushButton *okPushButton;
    TQPushButton *cancelPushButton;

protected slots:
    void chooseFont();

protected:
    TQVBoxLayout *optionsFormLayout;
    TQHBoxLayout *chartTypeLayout;
    TQHBoxLayout *fontLayout;
    TQVBoxLayout *addValuesFrameLayout;
    TQVBoxLayout *addValuesButtonGroupLayout;
    TQHBoxLayout *decimalPlacesLayout;
    TQHBoxLayout *buttonsLayout;

private:
    TQFont m_font;
};

#endif

