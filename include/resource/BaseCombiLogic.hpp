#ifndef __RESOURCE_BASE_COMBINATIONAL_LOGIC_HPP
#define __RESOURCE_BASE_COMBINATIONAL_LOGIC_HPP

#include "../base/Object.hpp"

class BaseCombiLogic: public Object{
    /***************************************************
     *  it is called base since it has the counter to  *
     *  count the running times for energy evaluation  *
     ***************************************************/
private:
    typedef uint32_t CountType;
    CountType _counter;

protected:
    inline void ResetCounter(){
        this->_counter=0;
        return;
    }
    inline void AddCounter(){
        this->_counter++;
        return;
    }

public:
    BaseCombiLogic(){
        this->_counter = 0;
        return;
    }
    inline CountType GetCounter(){
        return this->_counter;
    }
};

class Adder: public BaseCombiLogic{
    /** c = a + b **/
    class AdderParam{
    private:
        const Object::PortBitWidthType input0BW;
        const Object::PortBitWidthType input1BW;
        const Object::PortBitWidthType outputBW;
        const bool isSigned;

        Object::CounterType runTime;

        int number;

    public:
        AdderParam(
                Object::PortBitWidthType input0BW,
                Object::PortBitWidthType input1BW,
                Object::PortBitWidthType outputBW,
                bool isSigned
            ):input0BW(input0BW),
              input1BW(input1BW),
              outputBW(outputBW),
              isSigned(isSigned){
            this->runTime = 0;
            this->number  = 1;
            return;
        }
        inline bool Equal(
                Object::PortBitWidthType input0BW,
                Object::PortBitWidthType input1BW,
                Object::PortBitWidthType outputBW,
                bool isSigned
            )const{
            if ((this->input0BW == input0BW)
             && (this->input1BW == input1BW)
             && (this->outputBW == outputBW)
             && (this->isSigned == isSigned))
                return true;
            return false;
        }
        inline void Alloc(){
            this->number++;
            return;
        }
        inline void AddCounter(){
            this->runTime++;
            return;
        }
        inline void PrintPower(FILE* fp) const{
            if (this->isSigned){
                fprintf(fp,"\tAdder parameters: input0BW:%d input1BW:%d outputBW:%d signed\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW);
            }
            else{
                fprintf(fp,"\tAdder parameters: input0BW:%d input1BW:%d outputBW:%d unsigned\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW);
            }
            fprintf(fp,"\t\ttotal run times:\t%s\n",String::NumToString(this->runTime).c_str());
            return;
        }
        inline void PrintArea(FILE* fp) const{
            if (this->isSigned){
                fprintf(fp,"\tAdder parameters: input0BW:%d input1BW:%d outputBW:%d signed\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW);
            }
            else{
                fprintf(fp,"\tAdder parameters: input0BW:%d input1BW:%d outputBW:%d unsigned\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW);
            }
            fprintf(fp,"\t\ttotal numbers:\t%s\n",String::NumToString(this->number).c_str());
            return;
        }
    };

private:
    static vector<AdderParam> adderParam;
    static bool hasReserved;///temporary solution for adderParam

    int idx;

    bool isSigned;

    PortDataType lastInput0;
    PortDataType lastInput1;

    PortDataType c;
    PortBitWidthType input0BW;
    PortBitWidthType input1BW;
    PortBitWidthType outputBW;

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(Adder::PortName::PortNum());
        this->ports.emplace_back(input0BW,PortDirection::IN );
        this->ports.emplace_back(input1BW,PortDirection::IN );
        this->ports.emplace_back(outputBW,PortDirection::OUT);
        assert(this->ports.size()==Adder::PortName::PortNum());
        return;
    }

    inline void OutputConnect() override{
        this->ports[PORT_INDEX(output)].Clone(this->c);
        assert(this->OutputConnected());
        return;
    }

public:
    class PortName: public BasePortName{
    public:
        enum class _PortName:uint8_t{
            input0 = 0,
            input1 = 1,
            output = 2,
          /*************/
            PortNumber
        };
    private:
        _PortName portName;
    public:
        PortName(enum _PortName _portName):portName(_portName){
            assert(_portName != _PortName::PortNumber);
            return;
        }
        inline PortNumType ToIndex() const override{
            return static_cast<PortNumType>(this->portName);
        }
        static PortNumType PortNum(){
            return static_cast<PortNumType>(_PortName::PortNumber);
        }
        static inline PortNumType ToIndex(enum _PortName portName){
            assert(portName != _PortName::PortNumber);
            return static_cast<PortNumType>(portName);
        }
    };

    static const Adder::PortName in0;
    static const Adder::PortName in1;
    static const Adder::PortName out;

private:
    Adder(const Adder&);
    Adder &operator=(const Adder&);
public:
    Adder(PortBitWidthType input0BW,
          PortBitWidthType input1BW,
          PortBitWidthType outputBW,
          bool isSigned,
          PortDataType initC = 0){
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        assert(input0BW < input1BW);
        this-> input0BW = input0BW;
        this-> input1BW = input1BW;
        this-> outputBW = outputBW;
        this-> isSigned = isSigned;
        bool hasSame = false;
        if (!Adder::  hasReserved){
            ///temporary solution for Param
             Adder::adderParam.reserve(MAX_PARAM_SIZE);
             Adder::  hasReserved = true;
        }
        for (int i=0;i<Adder::adderParam.size();i++)
            if (Adder::adderParam[i].Equal(
                    this->input0BW,
                    this->input1BW,
                    this->outputBW,
                    this->isSigned)){
                Adder::adderParam[i].Alloc();
                this->idx = i;
                hasSame = true;
                break;
            }
        if (!hasSame){
            ///temporary solution for Param
            assert(Adder::adderParam.size()<=MAX_PARAM_SIZE);
            Adder::adderParam.emplace_back(
                    this->input0BW,
                    this->input1BW,
                    this->outputBW,
                    this->isSigned);
            this->idx=Adder::adderParam.size()-1;
        }

        this->c = initC;
        this->lastInput0 = 0;
        this->lastInput1 = 0;
        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }
    inline void InnerConnect() override{
        assert(this->InputConnected());
        return;
    }
    inline bool InnerConnected() const override{
        return true;
    }
    inline bool InnerConnectedCorrect() const override{
        return true;
    }
    inline bool OutputConnectedCorrect() const override{
        return this->ports[PORT_INDEX(output)].SameTo(this->c);
    }
    inline bool Run() override{
        assert(this->InputConnected());
        PortDataType tempC;

        if (this->lastInput0 == this->ports[PORT_INDEX(input0)].Data()
         && this->lastInput1 == this->ports[PORT_INDEX(input1)].Data()){
            return false;
        }

        this->lastInput0 = this->ports[PORT_INDEX(input0)].Data();
        this->lastInput1 = this->ports[PORT_INDEX(input1)].Data();

        this->ports[PORT_INDEX(input0)].Read();
        this->ports[PORT_INDEX(input1)].Read();

        Adder::adderParam[this->idx].AddCounter();
        if (!this->isSigned){
            tempC = this->ports[PORT_INDEX(input0)].Data()
                  + this->ports[PORT_INDEX(input1)].Data();
            if (this->c!= tempC){
                this->c = tempC;
                return true;
            }
            else
                return false;
        }
        else{
            tempC = this->ports[PORT_INDEX(input0)].SignedData()
                  + this->ports[PORT_INDEX(input1)].SignedData();
            if (this->c!= tempC){
                this->c = tempC;
                return true;
            }
            else
                return false;
        }
        return false;
    }
    inline void Print() const{
        std::cout<<"## in0: "<<this->ports[Adder::in0.ToIndex()].Data()<<std::endl;
        std::cout<<"## in1: "<<this->ports[Adder::in1.ToIndex()].Data()<<std::endl;
        std::cout<<"## out: "<<this->c<<std::endl;
    }
    static inline void PrintPower(FILE *fp){
        fprintf(fp,"Adder activation statistics:\n");
        for (int i=0;i<Adder::adderParam.size();i++)
            Adder::adderParam[i].PrintPower(fp);
        return;
    }
    static inline void PrintArea (FILE *fp){
        fprintf(fp,"Adder area statistics:\n");
        for (int i=0;i<Adder::adderParam.size();i++)
            Adder::adderParam[i].PrintArea(fp);
        return;
    }
    static inline void ResetStatis(){
        Adder::adderParam.clear();
        return;
    }
};

class Multer: public BaseCombiLogic{
    /** c = a * b **/
    class MulterParam{
    private:
        const Object::PortBitWidthType input0BW;
        const Object::PortBitWidthType input1BW;
        const Object::PortBitWidthType outputBW;
        const bool isSigned;

        Object::CounterType runTime;

        int number;

    public:
        MulterParam(
                Object::PortBitWidthType input0BW,
                Object::PortBitWidthType input1BW,
                Object::PortBitWidthType outputBW,
                bool isSigned
            ):input0BW(input0BW),
              input1BW(input1BW),
              outputBW(outputBW),
              isSigned(isSigned){
            this->runTime = 0;
            this->number  = 1;
            return;
        }
        inline bool Equal(
                Object::PortBitWidthType input0BW,
                Object::PortBitWidthType input1BW,
                Object::PortBitWidthType outputBW,
                bool isSigned
            )const{
            if ((this->input0BW == input0BW)
             && (this->input1BW == input1BW)
             && (this->outputBW == outputBW)
             && (this->isSigned == isSigned))
                return true;
            return false;
        }
        inline void Alloc(){
            this->number++;
            return;
        }
        inline void AddCounter(){
            this->runTime++;
            return;
        }
        inline void PrintPower(FILE* fp) const{
            if (this->isSigned){
                fprintf(fp,"\tMulter parameters: input0BW:%d input1BW:%d outputBW:%d signed\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW);
            }
            else{
                fprintf(fp,"\tMulter parameters: input0BW:%d input1BW:%d outputBW:%d unsigned\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW);
            }
            fprintf(fp,"\t\ttotal run times:\t%s\n",String::NumToString(this->runTime).c_str());
            return;
        }
        inline void PrintArea(FILE* fp) const{
            if (this->isSigned){
                fprintf(fp,"\tMulter parameters: input0BW:%d input1BW:%d outputBW:%d signed\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW);
            }
            else{
                fprintf(fp,"\tMulter parameters: input0BW:%d input1BW:%d outputBW:%d unsigned\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW);
            }
            fprintf(fp,"\t\ttotal numbers:\t%s\n",String::NumToString(this->number).c_str());
            return;
        }
    };
private:
    static vector<MulterParam> multerParam;
    static bool hasReserved;

    int idx;
    bool isSigned;

    PortDataType lastInput0;
    PortDataType lastInput1;

    PortDataType c;
    PortBitWidthType input0BW;
    PortBitWidthType input1BW;
    PortBitWidthType outputBW;

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(Multer::PortName::PortNum());
        this->ports.emplace_back(input0BW,PortDirection::IN );
        this->ports.emplace_back(input1BW,PortDirection::IN );
        this->ports.emplace_back(outputBW,PortDirection::OUT);
        assert(this->ports.size()==Multer::PortName::PortNum());
        return;
    }
    inline void OutputConnect() override{
        this->ports[PORT_INDEX(output)].Clone(this->c);
        assert(this->OutputConnected());
        return;
    }

public:
    class PortName: public BasePortName{
    public:
        enum class _PortName:uint8_t{
            input0 = 0,
            input1 = 1,
            output = 2,
          /*************/
            PortNumber
        };
    private:
        enum _PortName portName;
    public:
        PortName(enum _PortName portName):portName(portName){
            assert(portName != _PortName::PortNumber);
            return;
        }
        inline PortNumType ToIndex() const override{
            return static_cast<PortNumType>(this->portName);
        }
        static inline PortNumType PortNum(){
            return static_cast<PortNumType>(_PortName::PortNumber);
        }
        static inline PortNumType ToIndex(enum _PortName portName){
            assert(portName != _PortName::PortNumber);
            return static_cast<PortNumType>(portName);
        }
    };

    static const Multer::PortName in0;
    static const Multer::PortName in1;
    static const Multer::PortName out;

private:
    Multer(const Multer&);
    Multer &operator=(const Multer&);

public:
    Multer(PortBitWidthType input0BW,
           PortBitWidthType input1BW,
           PortBitWidthType outputBW,
           bool isSigned,
           PortDataType initC = 0){
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->input0BW = input0BW;
        this->input1BW = input1BW;
        this->outputBW = outputBW;
        this->isSigned = isSigned;
        this->c        = initC;

        this->lastInput0 = 0;
        this->lastInput1 = 0;

        if (!Multer::   hasReserved){
            ///temporary solution for Param
             Multer::multerParam.reserve(MAX_PARAM_SIZE);
             Multer::   hasReserved = true;
        }
        bool hasSame = false;
        for (int i=0;i<Multer::multerParam.size();i++)
            if (Multer::multerParam[i].Equal(
                    this->input0BW,
                    this->input1BW,
                    this->outputBW,
                    this->isSigned)){
                Multer::multerParam[i].Alloc();
                this->idx = i;
                hasSame = true;
                break;
            }
        if (!hasSame){
            ///temporary solution for Param
            assert(Multer::multerParam.size()<=MAX_PARAM_SIZE);
            Multer::multerParam.emplace_back(
                    this->input0BW,
                    this->input1BW,
                    this->outputBW,
                    this->isSigned);
            this->idx=Multer::multerParam.size()-1;
        }
        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }

    inline void InnerConnect() override{
        assert(this->InputConnected());
        return;
    }
    inline bool InnerConnected() const override{
        return true;
    }
    inline bool InnerConnectedCorrect() const override{
        return true;
    }
    inline bool OutputConnectedCorrect() const override{
        return this->ports[PORT_INDEX(output)].SameTo(this->c);
    }
    bool Run() override{
        PortDataType tempC;
        if (this->lastInput0 == this->ports[PORT_INDEX(input0)].Data()
         && this->lastInput1 == this->ports[PORT_INDEX(input1)].Data())
            return false;

        this->lastInput0 = this->ports[PORT_INDEX(input0)].Data();
        this->lastInput1 = this->ports[PORT_INDEX(input1)].Data();

        this->ports[PORT_INDEX(input0)].Read();
        this->ports[PORT_INDEX(input1)].Read();

        Multer::multerParam[this->idx].AddCounter();
        if (!this->isSigned){
            if (this->c == this->ports[PORT_INDEX(input0)].Data()
                         * this->ports[PORT_INDEX(input1)].Data())
                return false;
            this->c = this->ports[PORT_INDEX(input0)].Data()
                    * this->ports[PORT_INDEX(input1)].Data();
            return true;
        }
        else{
            tempC = this->ports[PORT_INDEX(input0)].SignedData()
                  * this->ports[PORT_INDEX(input1)].SignedData();
            if ( this->c == tempC)
                return false;
            this->c = tempC;
        }
        return true;
    }
    inline void Print() const{
        std::cout<<"## in0: "<<this->ports[Multer::in0.ToIndex()].Data()<<std::endl;
        std::cout<<"## in1: "<<this->ports[Multer::in1.ToIndex()].Data()<<std::endl;
        std::cout<<"## out: "<<this->c<<std::endl;
    }

    static inline void PrintPower(FILE *fp){
        fprintf(fp,"Multer activation statistics:\n");
        for (int i=0;i<Multer::multerParam.size();i++)
            Multer::multerParam[i].PrintPower(fp);
        return;
    }
    static inline void PrintArea (FILE *fp){
        fprintf(fp,"Multer area statistics:\n");
        for (int i=0;i<Multer::multerParam.size();i++)
            Multer::multerParam[i].PrintArea(fp);
        return;
    }
    static inline void ResetStatis(){
        Multer::multerParam.clear();
        return;
    }
};

class Comparer: public BaseCombiLogic{
    /** c = a comp b **/
    class CompParam{
    private:
        const Object::PortBitWidthType bit;

        Object::CounterType runTime;

        int number;

    public:
        CompParam(Object::PortBitWidthType bit)
            :bit(bit){
            this->runTime = 0;
            this->number  = 1;
            return;
        }
        inline bool Equal(Object::PortBitWidthType bit
            )const{
            return bit == this->bit;
        }
        inline void Alloc(){
            this->number++;
            return;
        }
        inline void AddCounter(){
            this->runTime++;
            return;
        }
        inline void PrintPower(FILE* fp) const{
            fprintf(fp,"\tComparer parameters: width:%d unsigned\n",
                        this->bit);
            fprintf(fp,"\t\ttotal run times:\t%s\n",String::NumToString(this->runTime).c_str());
            return;
        }
        inline void PrintArea(FILE* fp) const{
            fprintf(fp,"\tComparer parameters: width:%d unsigned\n",
                        this->bit);
            fprintf(fp,"\t\ttotal numbers:\t%s\n",String::NumToString(this->number).c_str());
            return;
        }
    };
private:
    static vector<CompParam> compParam;
    static bool hasReserved;

    int idx;

    PortDataType lastInput0;
    PortDataType lastInput1;

    PortDataType compResult;
    const PortBitWidthType bit;

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(Multer::PortName::PortNum());
        this->ports.emplace_back(this->bit,PortDirection::IN );
        this->ports.emplace_back(this->bit,PortDirection::IN );
        this->ports.emplace_back(2,PortDirection::OUT);
        assert(this->ports.size()==Multer::PortName::PortNum());
        return;
    }
    inline void OutputConnect() override{
        this->ports[PORT_INDEX(out)].Clone(this->compResult);
        assert(this->OutputConnected());
        return;
    }

public:
    class PortName: public BasePortName{
    public:
        enum class _PortName:uint8_t{
            in0 = 0,
            in1 = 1,
            out = 2,
          /*************/
            PortNumber
        };
    private:
        enum _PortName portName;
    public:
        PortName(enum _PortName portName):portName(portName){
            assert(portName != _PortName::PortNumber);
            return;
        }
        inline PortNumType ToIndex() const override{
            return static_cast<PortNumType>(this->portName);
        }
        static inline PortNumType PortNum(){
            return static_cast<PortNumType>(_PortName::PortNumber);
        }
        static inline PortNumType ToIndex(enum _PortName portName){
            assert(portName != _PortName::PortNumber);
            return static_cast<PortNumType>(portName);
        }
    };

    static const Comparer::PortName in0;
    static const Comparer::PortName in1;
    static const Comparer::PortName out;

private:
    Comparer(const Comparer&);
    Comparer &operator=(const Comparer&);

public:
    static constexpr PortDataType In0_Equal_In1 = 0;
    static constexpr PortDataType In0_Less_In1  = 1;
    static constexpr PortDataType In1_Less_In0  = 2;

    Comparer(PortBitWidthType bitWidth,
             PortDataType initResult = 0):bit(bitWidth){
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->compResult = initResult;

        this->lastInput0 = 0;
        this->lastInput1 = 0;

        if (!Comparer:: hasReserved){
            ///temporary solution for Param
             Comparer::compParam.reserve(MAX_PARAM_SIZE);
             Comparer:: hasReserved = true;
        }
        bool hasSame = false;
        for (int i=0;i<Comparer::compParam.size();i++)
            if (Comparer::compParam[i].Equal(this->bit)){
                Comparer::compParam[i].Alloc();
                this->idx =  i;
                hasSame = true;
                break;
            }
        if (!hasSame){
            ///temporary solution for Param
            assert(this->compParam.size()<=MAX_PARAM_SIZE);
            Comparer::compParam.emplace_back(this->bit);
            this->idx=Comparer::compParam.size()-1;
        }
        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }

    inline void InnerConnect() override{
        assert(this->InputConnected());
        return;
    }
    inline bool InnerConnected() const override{
        return true;
    }
    inline bool InnerConnectedCorrect() const override{
        return true;
    }
    inline bool OutputConnectedCorrect() const override{
        return this->ports[PORT_INDEX(out)].SameTo(this->compResult);
    }
    bool Run() override{
        PortDataType tempC;
        if (this->lastInput0 == this->ports[PORT_INDEX(in0)].Data()
         && this->lastInput1 == this->ports[PORT_INDEX(in1)].Data())
            return false;

        this->lastInput0 = this->ports[PORT_INDEX(in0)].Data();
        this->lastInput1 = this->ports[PORT_INDEX(in1)].Data();

        this->ports[PORT_INDEX(in0)].Read();
        this->ports[PORT_INDEX(in1)].Read();

        Comparer::compParam[this->idx].AddCounter();
        if (this->ports[PORT_INDEX(in0)].Data()<this->ports[PORT_INDEX(in1)].Data())
            this->compResult = Comparer:: In0_Less_In1;
        else if (this->ports[PORT_INDEX(in0)].Data()>this->ports[PORT_INDEX(in1)].Data())
            this->compResult = Comparer:: In1_Less_In0;
        else
            this->compResult = Comparer::In0_Equal_In1;
        return true;
    }
    inline void Print() const{
        std::cout<<"## in0: "<<this->ports[Comparer::in0.ToIndex()].Data()<<std::endl;
        std::cout<<"## in1: "<<this->ports[Comparer::in1.ToIndex()].Data()<<std::endl;
        std::cout<<"## out: "<<this->compResult<<std::endl;
    }

    static inline void PrintPower(FILE *fp){
        fprintf(fp,"Comparer activation statistics:\n");
        for (int i=0;i<Comparer::compParam.size();i++)
            Comparer::compParam[i].PrintPower(fp);
        return;
    }
    static inline void PrintArea (FILE *fp){
        fprintf(fp,"Comparer area statistics:\n");
        for (int i=0;i<Comparer::compParam.size();i++)
            Comparer::compParam[i].PrintArea(fp);
        return;
    }
    static inline void ResetStatis(){
        Comparer::compParam.clear();
        return;
    }
};


class ShiftAdder: public BaseCombiLogic{
    /// 00: output =  input0                       + input1
    /// 01: output = (input0<<          shiftBW0 ) + input1
    /// 10: output = (input0<< shiftBW1          ) + input1
    /// 11: output = (input0<<(shiftBW1+shiftBW0)) + input1
    class ShiftAdderParam{
    private:
        const Object::PortBitWidthType input0BW;
        const Object::PortBitWidthType input1BW;
        const Object::PortBitWidthType outputBW;
        const Object::PortBitWidthType  shiftBW0;
        const Object::PortBitWidthType  shiftBW1;
        const bool isSigned;

        Object::CounterType runTime;

        int number;

    public:
        ShiftAdderParam(
                Object::PortBitWidthType input0BW,
                Object::PortBitWidthType input1BW,
                Object::PortBitWidthType outputBW,
                Object::PortBitWidthType  shiftBW0,
                Object::PortBitWidthType  shiftBW1,
                bool isSigned
            ):input0BW (input0BW ),
              input1BW (input1BW ),
              outputBW (outputBW ),
               shiftBW0( shiftBW0),
               shiftBW1( shiftBW1),
              isSigned (isSigned ){
            this->runTime = 0;
            this->number  = 1;
            return;
        }
        inline bool Equal(
                Object::PortBitWidthType input0BW,
                Object::PortBitWidthType input1BW,
                Object::PortBitWidthType outputBW,
                Object::PortBitWidthType  shiftBW0,
                Object::PortBitWidthType  shiftBW1,
                bool isSigned
            )const{
            if ((this->input0BW == input0BW )
             && (this->input1BW == input1BW )
             && (this->outputBW == outputBW )
             && (this-> shiftBW0==  shiftBW0)
             && (this-> shiftBW1==  shiftBW1)
             && (this->isSigned == isSigned ))
                return true;
            return false;
        }
        inline void Alloc(){
            this->number++;
            return;
        }
        inline void AddCounter(){
            this->runTime++;
            return;
        }
        inline void PrintPower(FILE* fp) const{
            if (this->isSigned){
                fprintf(fp,"\tShift Adder parameters: input0BW:%d input1BW:%d outputBW:%d shiftBW0:%d shiftBW1:%d signed\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW,
                            this-> shiftBW0,
                            this-> shiftBW1);
            }
            else{
                fprintf(fp,"\tShift Adder parameters: input0BW:%d input1BW:%d outputBW:%d shiftBW0:%d shiftBW1:%d unsigned\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW,
                            this-> shiftBW0,
                            this-> shiftBW1);
            }
            fprintf(fp,"\t\ttotal run times:\t%s\n",String::NumToString(this->runTime).c_str());
            return;
        }
        inline void PrintArea(FILE* fp) const{
            if (this->isSigned){
                fprintf(fp,"\tShift Adder parameters: input0BW:%d input1BW:%d outputBW:%d shiftBW0:%d shiftBW1:%d signed\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW,
                            this-> shiftBW0,
                            this-> shiftBW1);
            }
            else{
                fprintf(fp,"\tShift Adder parameters: input0BW:%d input1BW:%d outputBW:%d shiftBW0:%d shiftBW1:%d unsigned\n",
                            this->input0BW,
                            this->input1BW,
                            this->outputBW,
                            this-> shiftBW0,
                            this-> shiftBW1);
            }
            fprintf(fp,"\t\ttotal numbers:\t%s\n",String::NumToString(this->number).c_str());
            return;
        }
    };

private:
    static vector<ShiftAdderParam> shiftAdderParam;
    static bool hasReserved;///temporary solution for adderParam

    int idx;

    bool isSigned;

    PortDataType lastInput0;
    PortDataType lastInput1;
    PortDataType lastShift ;
    PortDataType lastSign0 ;

    PortDataType c;
    PortBitWidthType input0BW;
    PortBitWidthType input1BW;
    PortBitWidthType outputBW;
    PortBitWidthType  shiftBW0;
    PortBitWidthType  shiftBW1;

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(ShiftAdder::PortName::PortNum());
        this->ports.emplace_back(input0BW,PortDirection::IN );
        this->ports.emplace_back(input1BW,PortDirection::IN );
        this->ports.emplace_back(outputBW,PortDirection::OUT);
        this->ports.emplace_back(   2    ,PortDirection::IN );
        this->ports.emplace_back(   1    ,PortDirection::IN );
        assert(this->ports.size()==ShiftAdder::PortName::PortNum());
        return;
    }

    inline void OutputConnect() override{
        this->ports[PORT_INDEX(output)].Clone(this->c);
        assert(this->OutputConnected());
        return;
    }

public:
    class PortName: public BasePortName{
    public:
        enum class _PortName:uint8_t{
            input0 = 0,
            input1 = 1,
            output = 2,
             shift = 3,
             sign0 = 4,
          /*************/
            PortNumber
        };
    private:
        _PortName portName;
    public:
        PortName(enum _PortName _portName):portName(_portName){
            assert(_portName != _PortName::PortNumber);
            return;
        }
        inline PortNumType ToIndex() const override{
            return static_cast<PortNumType>(this->portName);
        }
        static PortNumType PortNum(){
            return static_cast<PortNumType>(_PortName::PortNumber);
        }
        static inline PortNumType ToIndex(enum _PortName portName){
            assert(portName != _PortName::PortNumber);
            return static_cast<PortNumType>(portName);
        }
    };

    static const ShiftAdder::PortName   in0;
    static const ShiftAdder::PortName   in1;
    static const ShiftAdder::PortName   out;
    static const ShiftAdder::PortName shift;
    static const ShiftAdder::PortName sign0;

private:
    ShiftAdder(const ShiftAdder&);
    ShiftAdder &operator=(const ShiftAdder&);
public:
    ShiftAdder(
          PortBitWidthType input0BW,
          PortBitWidthType input1BW,
          PortBitWidthType outputBW,
          PortBitWidthType  shiftBW0,
          PortBitWidthType  shiftBW1,
          bool isSigned,
          PortDataType initC = 0){
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        assert(input0BW < input1BW);
        this-> input0BW = input0BW;
        this-> input1BW = input1BW;
        this-> outputBW = outputBW;
        this->  shiftBW0=  shiftBW0;
        this->  shiftBW1=  shiftBW1;
        this-> isSigned = isSigned;
        bool hasSame = false;
        if (!ShiftAdder::  hasReserved){
            ///temporary solution for Param
             ShiftAdder::shiftAdderParam.reserve(MAX_PARAM_SIZE);
             ShiftAdder::  hasReserved = true;
        }
        for (int i=0;i<ShiftAdder::shiftAdderParam.size();i++)
            if (ShiftAdder::shiftAdderParam[i].Equal(
                    this->input0BW,
                    this->input1BW,
                    this->outputBW,
                    this-> shiftBW0,
                    this-> shiftBW1,
                    this->isSigned)){
                ShiftAdder::shiftAdderParam[i].Alloc();
                this->idx = i;
                hasSame = true;
                break;
            }
        if (!hasSame){
            ///temporary solution for Param
            assert(ShiftAdder::shiftAdderParam.size()<=MAX_PARAM_SIZE);
            ShiftAdder::shiftAdderParam.emplace_back(
                    this->input0BW,
                    this->input1BW,
                    this->outputBW,
                    this-> shiftBW0,
                    this-> shiftBW1,
                    this->isSigned);
            this->idx=ShiftAdder::shiftAdderParam.size()-1;
        }

        this->c = initC;
        this->lastInput0 = 0;
        this->lastInput1 = 0;
        this->lastShift  = 0;
        this->lastSign0  = 0;
        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }
    inline void InnerConnect() override{
        assert(this->InputConnected());
        return;
    }
    inline bool InnerConnected() const override{
        return true;
    }
    inline bool InnerConnectedCorrect() const override{
        return true;
    }
    inline bool OutputConnectedCorrect() const override{
        return this->ports[PORT_INDEX(output)].SameTo(this->c);
    }
    inline bool Run() override{
        assert(this->InputConnected());
        PortDataType tempC,temp0,temp1;
        if (this->lastInput0 == this->ports[ShiftAdder::  in0.ToIndex()].Data()
         && this->lastInput1 == this->ports[ShiftAdder::  in1.ToIndex()].Data()
         && this->lastShift  == this->ports[ShiftAdder::shift.ToIndex()].Data()
         && this->lastSign0  == this->ports[ShiftAdder::sign0.ToIndex()].Data()){
            return false;
        }

        this->ports[ShiftAdder::  in0.ToIndex()].Read();
        this->ports[ShiftAdder::  in1.ToIndex()].Read();
        this->ports[ShiftAdder::shift.ToIndex()].Read();
        this->ports[ShiftAdder::sign0.ToIndex()].Read();

        this->lastInput0 = this->ports[ShiftAdder::  in0.ToIndex()].Data();
        this->lastInput1 = this->ports[ShiftAdder::  in1.ToIndex()].Data();
        this->lastShift  = this->ports[ShiftAdder::shift.ToIndex()].Data();
        this->lastSign0  = this->ports[ShiftAdder::sign0.ToIndex()].Data();

        if (!this->isSigned){
            temp0 = this->ports[PORT_INDEX(input0)].Data();
            temp1 = this->ports[PORT_INDEX(input1)].Data();
        }
        else{
            if (this->ports[ShiftAdder::sign0.ToIndex()].Data() == 1)
                temp0 = this->ports[PORT_INDEX(input0)].SignedData();
            else
                temp0 = this->ports[PORT_INDEX(input0)].Data();
            temp1 = this->ports[PORT_INDEX(input1)].SignedData();
        }

        ShiftAdder::shiftAdderParam[this->idx].AddCounter();

        switch(this->ports[shift.ToIndex()].Data()){
        case 0:/// 00: output =  input0                       + input1
            tempC = temp0
                  + temp1;
            break;
        case 1:/// 01: output = (input0<<          shiftBW0 ) + input1
            tempC =(temp0<< this->shiftBW0)
                  + temp1;
            break;
        case 2:/// 10: output = (input0<< shiftBW1          ) + input1
            tempC =(temp0<< this->shiftBW1)
                  + temp1;
            break;
        case 3:/// 11: output = (input0<<(shiftBW1+shiftBW0)) + input1
            tempC =(temp0<<(this->shiftBW1+this->shiftBW0))
                  + temp1;
            break;
        default:
            assert(false);
        }
        if (tempC == this->c)
            return false;
        this->c = tempC;
        return true;
    }
    inline void Print() const{
        std::cout<<"##   in0: "<<this->ports[ShiftAdder::  in0.ToIndex()].Data()<<std::endl;
        std::cout<<"##   in1: "<<this->ports[ShiftAdder::  in1.ToIndex()].Data()<<std::endl;
        std::cout<<"## shift: "<<this->ports[ShiftAdder::shift.ToIndex()].Data()<<std::endl;
        std::cout<<"##   out: "<<this->c<<std::endl;
    }
    static inline void PrintPower(FILE *fp){
        fprintf(fp,"Shift Adder activation statistics:\n");
        for (int i=0;i<ShiftAdder::shiftAdderParam.size();i++)
            ShiftAdder::shiftAdderParam[i].PrintPower(fp);
        return;
    }
    static inline void PrintArea (FILE *fp){
        fprintf(fp,"Shift Adder area statistics:\n");
        for (int i=0;i<ShiftAdder::shiftAdderParam.size();i++)
            ShiftAdder::shiftAdderParam[i].PrintArea(fp);
        return;
    }
    static inline void ResetStatis(){
        ShiftAdder::shiftAdderParam.clear();
        return;
    }
};

class SignCtrlMulter: public BaseCombiLogic{
    /** c = a * b **/
    class SignCtrlMulterParam{
    private:
        const Object::PortBitWidthType input0BW;
        const Object::PortBitWidthType input1BW;
        const Object::PortBitWidthType outputBW;

        Object::CounterType runTime;

        int number;

    public:
        SignCtrlMulterParam(
                Object::PortBitWidthType input0BW,
                Object::PortBitWidthType input1BW,
                Object::PortBitWidthType outputBW
            ):input0BW(input0BW),
              input1BW(input1BW),
              outputBW(outputBW){
            this->runTime = 0;
            this->number  = 1;
            return;
        }
        inline bool Equal(
                Object::PortBitWidthType input0BW,
                Object::PortBitWidthType input1BW,
                Object::PortBitWidthType outputBW
            )const{
            if ((this->input0BW == input0BW)
             && (this->input1BW == input1BW)
             && (this->outputBW == outputBW))
                return true;
            return false;
        }
        inline void Alloc(){
            this->number++;
            return;
        }
        inline void AddCounter(){
            this->runTime++;
            return;
        }
        inline void PrintPower(FILE* fp) const{
            fprintf(fp,"\tSignCtrlMulter parameters: input0BW:%d input1BW:%d outputBW:%d\n",
                        this->input0BW,
                        this->input1BW,
                        this->outputBW);
            fprintf(fp,"\t\ttotal run times:\t%s\n",String::NumToString(this->runTime).c_str());
            return;
        }
        inline void PrintArea(FILE* fp) const{
            fprintf(fp,"\tSignCtrlMulter parameters: input0BW:%d input1BW:%d outputBW:%d\n",
                        this->input0BW,
                        this->input1BW,
                        this->outputBW);
            fprintf(fp,"\t\ttotal numbers:\t%s\n",String::NumToString(this->number).c_str());
            return;
        }
    };
private:
    static vector<SignCtrlMulterParam> signCtrlMulterParam;
    static bool hasReserved;

    int idx;

    PortDataType lastInput0;
    PortDataType lastInput1;
    PortDataType lastSign0;
    PortDataType lastSign1;

    PortDataType c;
    PortBitWidthType input0BW;
    PortBitWidthType input1BW;
    PortBitWidthType outputBW;

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(SignCtrlMulter::PortName::PortNum());
        this->ports.emplace_back(input0BW,PortDirection::IN );
        this->ports.emplace_back(input1BW,PortDirection::IN );
        this->ports.emplace_back(outputBW,PortDirection::OUT);
        this->ports.emplace_back(    1   ,PortDirection::IN );
        this->ports.emplace_back(    1   ,PortDirection::IN );
        assert(this->ports.size()==SignCtrlMulter::PortName::PortNum());
        return;
    }
    inline void OutputConnect() override{
        this->ports[PORT_INDEX(output)].Clone(this->c);
        assert(this->OutputConnected());
        return;
    }

public:
    class PortName: public BasePortName{
    public:
        enum class _PortName:uint8_t{
            input0 = 0,
            input1 = 1,
            output = 2,
            sign0  = 3,
            sign1  = 4,
          /*************/
            PortNumber
        };
    private:
        enum _PortName portName;
    public:
        PortName(enum _PortName portName):portName(portName){
            assert(portName != _PortName::PortNumber);
            return;
        }
        inline PortNumType ToIndex() const override{
            return static_cast<PortNumType>(this->portName);
        }
        static inline PortNumType PortNum(){
            return static_cast<PortNumType>(_PortName::PortNumber);
        }
        static inline PortNumType ToIndex(enum _PortName portName){
            assert(portName != _PortName::PortNumber);
            return static_cast<PortNumType>(portName);
        }
    };

    static const SignCtrlMulter::PortName in0;
    static const SignCtrlMulter::PortName in1;
    static const SignCtrlMulter::PortName out;
    static const SignCtrlMulter::PortName sign0;
    static const SignCtrlMulter::PortName sign1;

private:
    SignCtrlMulter(const SignCtrlMulter&);
    SignCtrlMulter &operator=(const SignCtrlMulter&);

public:
    SignCtrlMulter(
           PortBitWidthType input0BW,
           PortBitWidthType input1BW,
           PortBitWidthType outputBW,
           PortDataType     initC = 0){
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->input0BW = input0BW;
        this->input1BW = input1BW;
        this->outputBW = outputBW;
        this->c        = initC;

        this->lastInput0 = 0;
        this->lastInput1 = 0;
        this->lastSign0  = 0;
        this->lastSign1  = 0;

        if (!SignCtrlMulter::   hasReserved){
            ///temporary solution for Param
             SignCtrlMulter::signCtrlMulterParam.reserve(MAX_PARAM_SIZE);
             SignCtrlMulter::   hasReserved = true;
        }
        bool hasSame = false;
        for (int i=0;i<SignCtrlMulter::signCtrlMulterParam.size();i++)
            if (SignCtrlMulter::signCtrlMulterParam[i].Equal(
                    this->input0BW,
                    this->input1BW,
                    this->outputBW)){
                SignCtrlMulter::signCtrlMulterParam[i].Alloc();
                this->idx = i;
                hasSame = true;
                break;
            }
        if (!hasSame){
            ///temporary solution for Param
            assert(SignCtrlMulter::signCtrlMulterParam.size()<=MAX_PARAM_SIZE);
            SignCtrlMulter::signCtrlMulterParam.emplace_back(
                    this->input0BW,
                    this->input1BW,
                    this->outputBW);
            this->idx=SignCtrlMulter::signCtrlMulterParam.size()-1;
        }
        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }

    inline void InnerConnect() override{
        assert(this->InputConnected());
        return;
    }
    inline bool InnerConnected() const override{
        return true;
    }
    inline bool InnerConnectedCorrect() const override{
        return true;
    }
    inline bool OutputConnectedCorrect() const override{
        return this->ports[PORT_INDEX(output)].SameTo(this->c);
    }
    bool Run() override{
        PortDataType tempC;
        if (this->lastInput0 == this->ports[PORT_INDEX(input0)].Data()
         && this->lastInput1 == this->ports[PORT_INDEX(input1)].Data()
         && this->lastSign0  == this->ports[PORT_INDEX( sign0)].Data()
         && this->lastSign1  == this->ports[PORT_INDEX( sign1)].Data())
            return false;

        this->lastInput0 = this->ports[PORT_INDEX(input0)].Data();
        this->lastInput1 = this->ports[PORT_INDEX(input1)].Data();
        this->lastSign0  = this->ports[PORT_INDEX( sign0)].Data();
        this->lastSign1  = this->ports[PORT_INDEX( sign1)].Data();

        this->ports[PORT_INDEX(input0)].Read();
        this->ports[PORT_INDEX(input1)].Read();
        this->ports[PORT_INDEX( sign0)].Read();
        this->ports[PORT_INDEX( sign1)].Read();

        SignCtrlMulter::signCtrlMulterParam[this->idx].AddCounter();

        PortDataType tempIn0,tempIn1;

        if (this->ports[SignCtrlMulter::sign0.ToIndex()].Data()==0)
            tempIn0 = this->ports[SignCtrlMulter::in0.ToIndex()].Data();
        else
            tempIn0 = this->ports[SignCtrlMulter::in0.ToIndex()].SignedData();

        if (this->ports[SignCtrlMulter::sign1.ToIndex()].Data()==0)
            tempIn1 = this->ports[SignCtrlMulter::in1.ToIndex()].Data();
        else
            tempIn1 = this->ports[SignCtrlMulter::in1.ToIndex()].SignedData();

        if (this->c == tempIn0 * tempIn1)
            return false;
        this->c = tempIn0 * tempIn1;
        return true;
    }
    inline void Print() const{
        std::cout<<"##  in0 : "<<this->ports[SignCtrlMulter::  in0.ToIndex()].Data()<<std::endl;
        std::cout<<"##  in1 : "<<this->ports[SignCtrlMulter::  in1.ToIndex()].Data()<<std::endl;
        std::cout<<"## sign0: "<<this->ports[SignCtrlMulter::sign0.ToIndex()].Data()<<std::endl;
        std::cout<<"## sign1: "<<this->ports[SignCtrlMulter::sign1.ToIndex()].Data()<<std::endl;
        std::cout<<"##  out : "<<this->c<<std::endl;
    }

    static inline void PrintPower(FILE *fp){
        fprintf(fp,"SignCtrlMulter activation statistics:\n");
        for (int i=0;i<SignCtrlMulter::signCtrlMulterParam.size();i++)
            SignCtrlMulter::signCtrlMulterParam[i].PrintPower(fp);
        return;
    }
    static inline void PrintArea (FILE *fp){
        fprintf(fp,"SignCtrlMulter area statistics:\n");
        for (int i=0;i<SignCtrlMulter::signCtrlMulterParam.size();i++)
            SignCtrlMulter::signCtrlMulterParam[i].PrintArea(fp);
        return;
    }
    static inline void ResetStatis(){
        SignCtrlMulter::signCtrlMulterParam.clear();
        return;
    }
};

#endif // __RESOURCE_BASE_COMBINATIONAL_LOGIC_HPP
