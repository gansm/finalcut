#include <final/final.h>

namespace fc = finalcut::fc;
using finalcut::FColorPair;
using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class TextWindow
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class SegmentView : public finalcut::FDialog
{
  public:
    explicit SegmentView (finalcut::FWidget* = nullptr);

  private:
    // Typedef
    typedef struct
    {
      unsigned char a : 1;
      unsigned char b : 1;
      unsigned char c : 1;
      unsigned char d : 1;
      unsigned char e : 1;
      unsigned char f : 1;
      unsigned char g : 1;
      unsigned char   : 1;  // padding bit
    } sevenSegment;

    // Methods
    void hexEncoding();
    void get7Segment (const wchar_t);
    virtual void draw() override;

    // Data Members
    std::map<wchar_t, sevenSegment> code{};
    finalcut::FString line[3];
    finalcut::FLineEdit Input{"0123", this};
    finalcut::FButton Exit{"E&xit", this};
};
#pragma pack(pop)

//----------------------------------------------------------------------
SegmentView::SegmentView (finalcut::FWidget* parent)
  : FDialog(parent)
{
  // Set encoding
  hexEncoding();

  // Dialog settings
  setText ("Seven-segment display");
  setGeometry (FPoint(25, 5), FSize(42, 15));

  // Input field
  Input.setGeometry (FPoint(2, 2), FSize(12, 1));
  Input.setLabelText (L"&Hex value");
  Input.setLabelText (L"&Hex-digits or (.) (:) (H) (L) (P) (U)");
  Input.setLabelOrientation(finalcut::FLineEdit::label_above);
  Input.setMaxLength(9);
  Input.setInputFilter("[:.hHlLpPuU[:xdigit:]]");

  // Exit button
  Exit.setGeometry(FPoint(28, 11), FSize(10, 1));

  // Add some function callbacks
  Input.addCallback
  (
    "changed",
    [] (finalcut::FWidget*, FDataPtr data)
    {
      auto dialog = static_cast<SegmentView*>(data);
      dialog->redraw();
    },
    this
  );

  Exit.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &finalcut::FApplication::cb_exitApp)
  );
}

//----------------------------------------------------------------------
void SegmentView::hexEncoding()
{
  code['0'] = sevenSegment{1, 1, 1, 1, 1, 1, 0};
  code['1'] = sevenSegment{0, 1, 1, 0, 0, 0, 0};
  code['2'] = sevenSegment{1, 1, 0, 1, 1, 0, 1};
  code['3'] = sevenSegment{1, 1, 1, 1, 0, 0, 1};
  code['4'] = sevenSegment{0, 1, 1, 0, 0, 1, 1};
  code['5'] = sevenSegment{1, 0, 1, 1, 0, 1, 1};
  code['6'] = sevenSegment{1, 0, 1, 1, 1, 1, 1};
  code['7'] = sevenSegment{1, 1, 1, 0, 0, 0, 0};
  code['8'] = sevenSegment{1, 1, 1, 1, 1, 1, 1};
  code['9'] = sevenSegment{1, 1, 1, 1, 0, 1, 1};
  code['A'] = sevenSegment{1, 1, 1, 0, 1, 1, 1};
  code['B'] = sevenSegment{0, 0, 1, 1, 1, 1, 1};
  code['C'] = sevenSegment{1, 0, 0, 1, 1, 1, 0};
  code['D'] = sevenSegment{0, 1, 1, 1, 1, 0, 1};
  code['E'] = sevenSegment{1, 0, 0, 1, 1, 1, 1};
  code['F'] = sevenSegment{1, 0, 0, 0, 1, 1, 1};
}

//----------------------------------------------------------------------
void SegmentView::get7Segment (const wchar_t c)
{
  sevenSegment& s = code[c];
  constexpr char h[2]{' ', '_'};
  constexpr char v[2]{' ', '|'};

  for (int i = 0; i < 3; i++)
    line[i].clear();

  switch ( c )
  {
    case ':':
      line[0] = ' ';
      line[1] = '.';
      line[2] = '.';
      break;

    case '.':
      line[0] = ' ';
      line[1] = ' ';
      line[2] = '.';
      break;

    case 'H':
      line[0] = "   ";
      line[1] = "|_|";
      line[2] = "| |";
      break;

    case 'L':
      line[0] = "   ";
      line[1] = "|  ";
      line[2] = "|_ ";
      break;

    case 'P':
      line[0] = " _ ";
      line[1] = "|_|";
      line[2] = "|  ";
      break;

    case 'U':
      line[0] = "   ";
      line[1] = "| |";
      line[2] = "|_|";
      break;

    default:
      // Hexadecimal digit from 0 up to f
      line[0] <<   ' '  << h[s.a] <<   ' ';
      line[1] << v[s.f] << h[s.g] << v[s.b];
      line[2] << v[s.e] << h[s.d] << v[s.c];
  }
}

//----------------------------------------------------------------------
void SegmentView::draw()
{
  std::vector<finalcut::FTermBuffer> tbuffer{3};
  finalcut::FTermBuffer left_space{};

  FDialog::draw();
  setColor(fc::LightGray, fc::Black);
  FWidget::drawBorder(3, 6, 40, 11);

  for (auto&& ch : Input.getText().toUpper())
  {
    FColorPair color(fc::LightRed, fc::Black);
    get7Segment(ch);

    for (std::size_t i = 0; i < 3; i++)
      tbuffer[i] << color << line[i] << " ";
  }

  std::size_t length = tbuffer[0].getLength();

  if ( length < 36 )
    left_space << finalcut::FString(36 - length, ' ');

  print() << FPoint (4, 7) << left_space << tbuffer[0]
          << FPoint (4, 8) << left_space << tbuffer[1]
          << FPoint (4, 9) << left_space << tbuffer[2]
          << FPoint (4, 10) << finalcut::FString(36, ' ');
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  finalcut::FApplication app(argc, argv);
  SegmentView dialog(&app);
  app.setMainWidget(&dialog);
  dialog.show();
  return app.exec();
}
