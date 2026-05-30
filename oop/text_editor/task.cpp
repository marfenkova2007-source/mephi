#include "logged_command_wrapper.h"
#include <iostream>
#include <list>
#include <vector>

/* Курсор влево */
class MoveCursorLeftCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        if (cursorPosition>0){
            --cursorPosition;
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveCursorLeftCommand(*this);
    }
};

/* Курсор вправо */
class MoveCursorRightCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        if (buffer.size()>cursorPosition){
            ++cursorPosition;
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveCursorRightCommand(*this);
    }
};

class MoveCursorUpCommand : public ICommand {
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        int k = 0;
        size_t nach_curs = cursorPosition;
        while (nach_curs>0 and !(buffer[nach_curs-1]=='\n')){
            ++k;
            --nach_curs;
        }
        if (nach_curs>0){
            int len_of_new_string = 0;
            --nach_curs;
            while (nach_curs>0 and !(buffer[nach_curs-1]=='\n')){
                ++len_of_new_string;
                --nach_curs;
            }
            if (len_of_new_string>=k){
                cursorPosition = nach_curs + k;
            }
            else{
                cursorPosition= nach_curs+ len_of_new_string;
            }
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveCursorUpCommand(*this);
    }
};
 
/* Курсор вниз */
class MoveCursorDownCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        int kol_bukv_do_nach = 0;
        if (cursorPosition<buffer.size() and buffer[cursorPosition]=='\n'){
            if (cursorPosition+1<buffer.size()){
                size_t new_cursor = cursorPosition;
                while (new_cursor>0 and buffer[new_cursor-1]!='\n'){
                    --new_cursor;
                }
                auto rast = cursorPosition-new_cursor;
                size_t next_cursor = cursorPosition +1;
                while (next_cursor< buffer.size() and buffer[next_cursor] != '\n'){
                    ++next_cursor;
                }
                ++cursorPosition ;
                cursorPosition += std::min( rast, next_cursor - cursorPosition);
            }
            return;
        }
        size_t nach_curs = cursorPosition;
        while (nach_curs>0 and !(buffer[nach_curs-1]=='\n')){
            ++kol_bukv_do_nach;
            --nach_curs;
        }
        while (cursorPosition<buffer.size() and !(buffer[cursorPosition]=='\n')){
            ++cursorPosition;
        }
        if (cursorPosition<buffer.size()){
            ++cursorPosition;
            while (kol_bukv_do_nach!=0 and !(buffer[cursorPosition]=='\n') and cursorPosition<buffer.size()){
                --kol_bukv_do_nach;
                ++cursorPosition;
            }
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveCursorDownCommand(*this);
    }
};

/* Выделить текст */
class SelectTextCommand : public ICommand {
public:
    SelectTextCommand(size_t selectionSize): selectionSize_ (selectionSize){}
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.SelectText(cursorPosition, std::min(cursorPosition + selectionSize_, buffer.size()));
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitSelectCommand(*this);
    }
private:
    size_t selectionSize_;
};

/* Ввести текст */
class InsertTextCommand : public ICommand {
public:
    InsertTextCommand(std::string text): new_text(text){}
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            auto [start, end]  = editor.GetSelection();
            buffer.erase(start, end-start);
            cursorPosition = start;
        }
        editor.UnselectText();
        buffer.insert(cursorPosition, new_text);
        cursorPosition += new_text.length();
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitInsertTextCommand(*this);
    }
private:
    std::string new_text;
};

/* Удалить текст */
class DeleteTextCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            auto [start, end]  = editor.GetSelection();
            buffer.erase(start, end-start);
            cursorPosition = start;
            editor.UnselectText();
        }
        else{
            if (cursorPosition<buffer.size()){
                buffer.erase(cursorPosition, 1);
            }
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitDeleteTextCommand(*this);
    }

};

/* Скопировать текст */
class CopyTextCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            clipboard = "";
            auto [start, end] = editor.GetSelection();
            for (; start<end; ++start){
                clipboard += buffer[start];
            }
        }
        else{
            clipboard = buffer[cursorPosition];
        }
        editor.UnselectText();
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitCopyTextCommand(*this);
    }
};

/* Вставить скопированный текст */
class PasteTextCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            auto [start, end]  = editor.GetSelection();
            buffer.erase(start, end-start);
            cursorPosition = start;
        }
        buffer.insert(cursorPosition, clipboard);
        cursorPosition += clipboard.length();
        editor.UnselectText();
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitPasteTextCommand(*this);
    }
};

/* Привести выделенный текст в ВЕРХНИЙ регистр */
class UppercaseTextCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            auto [start, end]  = editor.GetSelection();
            for (; start<end; ++start){
                buffer[start] = std::toupper(buffer[start]);
            }
        }
        editor.UnselectText();
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitUppercaseTextCommand(*this);
    }
};

/* Привести выделенный текст в нижний регистр */
class LowercaseTextCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            auto [start, end]  = editor.GetSelection();
            for (; start<end; ++start){
                buffer[start] = std::tolower(buffer[start]);
            }
        }
        editor.UnselectText();
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitLowercaseTextCommand(*this);
    }
};

/* Перенести курсор в конец строки */
class MoveToEndCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        while (cursorPosition<buffer.size() and buffer[cursorPosition]!='\n'){
            ++cursorPosition;
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveToEndCommand(*this);
    }
};

/* Перенести курсор в начало строки */
class MoveToStartCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        while (cursorPosition>0 and buffer[cursorPosition-1]!='\n'){
            --cursorPosition;
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveToStartCommand(*this);
    } 
};

/* Удалить часть строки, начиная от позиции курсора до первого пробела или конца строки */
class DeleteWordCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        while (cursorPosition<buffer.size() and buffer[cursorPosition]!=' ' and buffer[cursorPosition]!= '\n'){
            buffer.erase(cursorPosition, 1);
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitDeleteWordCommand(*this);
    }
};

/* Макрос */
class MacroCommand : public ICommand {
public:
    MacroCommand(std::list<CommandPtr> commands) : macrocommands_(commands.begin(), commands.end()) {}
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        for (auto& c: macrocommands_){
            c->Apply(buffer, cursorPosition, clipboard, editor);
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        //visitor.VisitMacroCommand(*this);
    }
private:
    std::vector<CommandPtr> macrocommands_;
};

CommandLoggerVisitor::CommandLoggerVisitor(std::ostream& logStream): logStream_(logStream){}
void CommandLoggerVisitor::VisitMoveCursorLeftCommand(MoveCursorLeftCommand& command){
    logStream_<<"h";
}
void CommandLoggerVisitor::VisitMoveCursorRightCommand(MoveCursorRightCommand& command){
    logStream_<<"l";
}
void CommandLoggerVisitor::VisitMoveCursorUpCommand(MoveCursorUpCommand& command){
    logStream_<<"k";
}
void CommandLoggerVisitor::VisitMoveCursorDownCommand(MoveCursorDownCommand& command){
    logStream_<<"j";
}
void CommandLoggerVisitor::VisitSelectCommand(SelectTextCommand& command){
    logStream_<<"v";
}
void CommandLoggerVisitor::VisitInsertTextCommand(InsertTextCommand& command){
    logStream_<<"i";
}
void CommandLoggerVisitor::VisitDeleteTextCommand(DeleteTextCommand& command){
    logStream_<<"d";
}
void CommandLoggerVisitor::VisitCopyTextCommand(CopyTextCommand& command){
    logStream_<<"y";
}
void CommandLoggerVisitor::VisitPasteTextCommand(PasteTextCommand& command){
    logStream_<<"p";
}
void CommandLoggerVisitor::VisitUppercaseTextCommand(UppercaseTextCommand& command){
    logStream_<<"U";
}
void CommandLoggerVisitor::VisitLowercaseTextCommand(LowercaseTextCommand& command){
    logStream_<<"u";
}
void CommandLoggerVisitor::VisitMoveToEndCommand(MoveToEndCommand& command){
    logStream_<<"$";
}
void CommandLoggerVisitor::VisitMoveToStartCommand(MoveToStartCommand& command){
    logStream_<<"0";
}
void CommandLoggerVisitor::VisitDeleteWordCommand(DeleteWordCommand& command){
    logStream_<<"dE";
}


CommandBuilder::CommandBuilder(): 
    type_ (Type::MoveCursorLeft),
    selectionSize_(0),
    text_(""),
    logStreamPtr_(nullptr),
    subcommands_(){}

    CommandPtr CommandBuilder::build(){
        if (type_==Type::MoveCursorLeft){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveCursorLeftCommand>());
            }
            return std::make_shared<MoveCursorLeftCommand>();
        }
        if (type_==Type::MoveCursorRight){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveCursorRightCommand>());
            }
            return std::make_shared<MoveCursorRightCommand>();
        }
        if (type_==Type::MoveCursorUp){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveCursorUpCommand>());
            }
            return std::make_shared<MoveCursorUpCommand>();
        }
        if (type_==Type::MoveCursorDown){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveCursorDownCommand>());
            }
            return std::make_shared<MoveCursorDownCommand>();
        }
        if (type_==Type::SelectText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<SelectTextCommand>(selectionSize_));
            }
            return std::make_shared<SelectTextCommand>(selectionSize_);
        }
        if (type_==Type::InsertText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<InsertTextCommand>(text_));
            }
            return std::make_shared<InsertTextCommand>(text_);
        }
        if (type_==Type::DeleteText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<DeleteTextCommand>());
            }
            return std::make_shared<DeleteTextCommand>();
        }
        if (type_==Type::CopyText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<CopyTextCommand>());
            }
            return std::make_shared<CopyTextCommand>();
        }
        if (type_==Type::PasteText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<PasteTextCommand>());
            }
            return std::make_shared<PasteTextCommand>();
        }
        if (type_==Type::UppercaseText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<UppercaseTextCommand>());
            }
            return std::make_shared<UppercaseTextCommand>();
        }
        if (type_==Type::LowercaseText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<LowercaseTextCommand>());
            }
            return std::make_shared<LowercaseTextCommand>();
        }
        if (type_==Type::MoveToEnd){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveToEndCommand>());
            }
            return std::make_shared<MoveToEndCommand>();
        }
        if (type_==Type::MoveToStart){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveToStartCommand>());
            }
            return std::make_shared<MoveToStartCommand>();
        }
        if (type_==Type::DeleteWord){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<DeleteWordCommand>());
            }
            return std::make_shared<DeleteWordCommand>();
        }
        if (type_==Type::Macro){
            if (logStreamPtr_){
                std::list<CommandPtr> comands;
                for (auto& comand: subcommands_){
                    comands.push_back(std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, comand));
                }
                return std::make_shared<MacroCommand>(comands);
            }
            return std::make_shared<MacroCommand>(subcommands_);
        }
        return {};
    }
    CommandBuilder& CommandBuilder::WithType(Type type){
        type_ = type;
        return *this;
    };
    CommandBuilder& CommandBuilder::SelectionSize(size_t selectionSize){
        selectionSize_ = selectionSize;
        return *this;
    };
    CommandBuilder& CommandBuilder::Text(std::string text){
        text_ = text;
        return *this;
    };
    CommandBuilder& CommandBuilder::LogTo(std::ostream& logStream){
        logStreamPtr_ = &logStream;
        return *this;
    };
    CommandBuilder& CommandBuilder::AddSubcommand(CommandPtr subcommand){
        subcommands_.push_back(subcommand);
        return *this;
    };
