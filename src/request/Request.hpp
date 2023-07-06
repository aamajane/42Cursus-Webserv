# pragma once

#include "../webserv.hpp"
class MimeTypes;

enum Methods {
    GET,
    POST,
    DELETE
};

enum state
{
    Request_Line,
    Request_Line_Method,
    Request_Line_First_Space,
    Request_Line_URI_Path_Slash,
    Request_Line_URI_Path,
    Request_Line_URI_Query,
    Request_Line_Ver,
    Request_Line_HT,
    Request_Line_HTT,
    Request_Line_HTTP,
    Request_Line_HTTP_Slash,
    Request_Line_Major,
    Request_Line_Dot,
    Request_Line_Minor,
    Request_Line_CR,
    Request_Line_LF,
    Field_Name_Start,
    Field_End,
    Field_Name,
    Field_Value,
    Field_Value_End,
    Chunked_Length_Begin,
    Chunked_Length,
    Chunked_Ignore,
    Chunked_Length_CR,
    Chunked_Length_LF,
    Chunked_Data,
    Chunked_Data_CR,
    Chunked_Data_LF,
    Chunked_End_CR,
    Chunked_End_LF,
    Message_Body,
    Parsing_Done
};

class Request {
    private :
        std::string                             Path;
        std::string                             Query;
        std::map<std::string, std::string>      requestHeaders;
        std::vector<u_int8_t>                   Body;
        Methods                                 Method;
        std::map<u_int8_t, std::string>         methodsString;
        state                                   State;
        size_t                                  bodySize;
        size_t                                  maxBodySize;
        size_t                                  errorCode;
        size_t                                  chunkedLength;
        std::string                             Storage;
        std::string                             keyStorage;
        short                                   methodIndex;
        u_int8_t                                verMajor;
        u_int8_t                                verMinor;
        std::string                             Host;
        uint16_t                                Port;
        int                                     fdFileBody;
        std::string                             nameFileBody;
        bool                                    bodyFlag;
        bool                                    chunkedFlag;
        int                                     indexBuffer;
        u_int8_t                                c;

        bool                                   handleHeaders();

        bool                                   openFile();

    public :
        static MimeTypes mime;

        Request();

        ~Request();

        Request (const Request & );

        Request &operator= (const Request &);

        void                    clear();

        // ? ----------------------------- getters -----------------------------------

        const std::string &                             getPath() const;

        const std::string &                             getQuery() const ;

        const std::map<std::string, std::string>&       getrequestHeaders() const;

        const std::string   &                           getHeader(std::string );

        Methods                                         getMethod() const;

        const std::string&                              getMethodsString();

        short                                           getCodeError() const;

        const std::string &                             getHost() const;

        state                                           getState() const;

        uint16_t                                        getPort() const;

        int                                             getFdFileBody() const;

        const std::string &                             getNameFileBody() const;

        // ? ----------------------------- setters -----------------------------------

        void        setHeader(std::string &, std::string &);

        void        setMethod(Methods &);

        void        setMaxBodySize(size_t);

        void        setCodeError(short );


        // ? Methods ---------------------------------------------------------------- 

        bool                            keepAlive();
        
        void                            readBufferFromReq(char *, int);

        void                            printRequest();
        
        std::string                     getNewFileName(std::string );
};