/*
www.sourceforge.net/projects/tinyxml
Original code (2.0 and earlier )copyright (c) 2000-2006 Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/
#include "..\stdafx.h"
#include "tinyxml.h"

#include "..\vfs\vfs.h"
#include "..\console\console_gui.h"

namespace vEngine{


bool XmlBase::condenseWhiteSpace = true;

// Microsoft compiler security
FILE* XmlFOpen( const TCHAR* filename, const TCHAR* mode )
{
/*	#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
		FILE* fp = 0;
		errno_t err = fopen_s( &fp, filename, mode );
		if ( !err && fp )
			return fp;
		return 0;
	#else*/
		return _tfopen( filename, mode );
	//#endif
}

void XmlBase::EncodeString( const std::string& str, std::string* outString )
{
	int i=0;

	while( i<(int)str.length() )
	{
		unsigned char c = (unsigned char) str[i];

		if (    c == '&' 
		     && i < ( (int)str.length() - 2 )
			 && str[i+1] == '#'
			 && str[i+2] == 'x' )
		{
			// Hexadecimal character reference.
			// Pass through unchanged.
			// &#xA9;	-- copyright symbol, for example.
			//
			// The -1 is a bug fix from Rob Laveaux. It keeps
			// an overflow from happening if there is no ';'.
			// There are actually 2 ways to exit this loop -
			// while fails (error case) and break (semicolon found).
			// However, there is no mechanism (currently) for
			// this function to return an error.
			while ( i<(int)str.length()-1 )
			{
				outString->append( str.c_str() + i, 1 );
				++i;
				if ( str[i] == ';' )
					break;
			}
		}
		else if ( c == '&' )
		{
			outString->append( entity[0].str, entity[0].strLength );
			++i;
		}
		else if ( c == '<' )
		{
			outString->append( entity[1].str, entity[1].strLength );
			++i;
		}
		else if ( c == '>' )
		{
			outString->append( entity[2].str, entity[2].strLength );
			++i;
		}
		else if ( c == '\"' )
		{
			outString->append( entity[3].str, entity[3].strLength );
			++i;
		}
		else if ( c == '\'' )
		{
			outString->append( entity[4].str, entity[4].strLength );
			++i;
		}
		else if ( c < 32 )
		{
			// Easy pass at non-alpha/numeric/symbol
			// Below 32 is symbolic.
			char buf[ 32 ];
			
			#if defined(TIXML_SNPRINTF)		
				TIXML_SNPRINTF( buf, sizeof(buf), "&#x%02X;", (unsigned) ( c & 0xff ) );
			#else
				sprintf( buf, "&#x%02X;", (unsigned) ( c & 0xff ) );
			#endif		

			//*ME:	warning C4267: convert 'size_t' to 'int'
			//*ME:	Int-Cast to make compiler happy ...
			outString->append( buf, (int)strlen( buf ) );
			++i;
		}
		else
		{
			//char realc = (char) c;
			//outString->append( &realc, 1 );
			*outString += (char) c;	// somewhat more efficient function call.
			++i;
		}
	}
}


XmlNode::XmlNode( NodeType _type ) : XmlBase()
{
	parent = 0;
	type = _type;
	firstChild = 0;
	lastChild = 0;
	prev = 0;
	next = 0;
}


XmlNode::~XmlNode()
{
	XmlNode* node = firstChild;
	XmlNode* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->next;
		delete(temp);
	}	
}


void XmlNode::CopyTo( XmlNode* target ) const
{
	target->SetValue (value.c_str() );
	target->userData = userData; 
}


void XmlNode::Clear()
{
	XmlNode* node = firstChild;
	XmlNode* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->next;
		delete(temp);
	}	

	firstChild = 0;
	lastChild = 0;
}


XmlNode* XmlNode::LinkEndChild( XmlNode* node )
{
	assert( node->parent == 0 || node->parent == this );
	assert( node->GetDocument() == 0 || node->GetDocument() == this->GetDocument() );

	if ( node->Type() == XmlNode::DOCUMENT )
	{
		delete(node);
		if ( GetDocument() ) GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	node->parent = this;

	node->prev = lastChild;
	node->next = 0;

	if ( lastChild )
		lastChild->next = node;
	else
		firstChild = node;			// it was an empty list.

	lastChild = node;
	return node;
}


XmlNode* XmlNode::InsertEndChild( const XmlNode& addThis )
{
	if ( addThis.Type() == XmlNode::DOCUMENT )
	{
		if ( GetDocument() ) GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}
	XmlNode* node = addThis.Clone();
	if ( !node )
		return 0;

	return LinkEndChild( node );
}


XmlNode* XmlNode::InsertBeforeChild( XmlNode* beforeThis, const XmlNode& addThis )
{	
	if ( !beforeThis || beforeThis->parent != this ) {
		return 0;
	}
	if ( addThis.Type() == XmlNode::DOCUMENT )
	{
		if ( GetDocument() ) GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	XmlNode* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;

	node->next = beforeThis;
	node->prev = beforeThis->prev;
	if ( beforeThis->prev )
	{
		beforeThis->prev->next = node;
	}
	else
	{
		assert( firstChild == beforeThis );
		firstChild = node;
	}
	beforeThis->prev = node;
	return node;
}


XmlNode* XmlNode::InsertAfterChild( XmlNode* afterThis, const XmlNode& addThis )
{
	if ( !afterThis || afterThis->parent != this ) {
		return 0;
	}
	if ( addThis.Type() == XmlNode::DOCUMENT )
	{
		if ( GetDocument() ) GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	XmlNode* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;

	node->prev = afterThis;
	node->next = afterThis->next;
	if ( afterThis->next )
	{
		afterThis->next->prev = node;
	}
	else
	{
		assert( lastChild == afterThis );
		lastChild = node;
	}
	afterThis->next = node;
	return node;
}


XmlNode* XmlNode::ReplaceChild( XmlNode* replaceThis, const XmlNode& withThis )
{
	if ( replaceThis->parent != this )
		return 0;

	XmlNode* node = withThis.Clone();
	if ( !node )
		return 0;

	node->next = replaceThis->next;
	node->prev = replaceThis->prev;

	if ( replaceThis->next )
		replaceThis->next->prev = node;
	else
		lastChild = node;

	if ( replaceThis->prev )
		replaceThis->prev->next = node;
	else
		firstChild = node;

	delete(replaceThis);
	node->parent = this;
	return node;
}


bool XmlNode::RemoveChild( XmlNode* removeThis )
{
	if ( removeThis->parent != this )
	{	
		assert( 0 );
		return false;
	}

	if ( removeThis->next )
		removeThis->next->prev = removeThis->prev;
	else
		lastChild = removeThis->prev;

	if ( removeThis->prev )
		removeThis->prev->next = removeThis->next;
	else
		firstChild = removeThis->next;

	delete(removeThis);
	return true;
}

const XmlNode* XmlNode::FirstChild( const char * _value ) const
{
	const XmlNode* node;
	for ( node = firstChild; node; node = node->next )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}


const XmlNode* XmlNode::LastChild( const char * _value ) const
{
	const XmlNode* node;
	for ( node = lastChild; node; node = node->prev )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}


const XmlNode* XmlNode::IterateChildren( const XmlNode* previous ) const
{
	if ( !previous )
	{
		return FirstChild();
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling();
	}
}


const XmlNode* XmlNode::IterateChildren( const char * val, const XmlNode* previous ) const
{
	if ( !previous )
	{
		return FirstChild( val );
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling( val );
	}
}


const XmlNode* XmlNode::NextSibling( const char * _value ) const 
{
	const XmlNode* node;
	for ( node = next; node; node = node->next )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}


const XmlNode* XmlNode::PreviousSibling( const char * _value ) const
{
	const XmlNode* node;
	for ( node = prev; node; node = node->prev )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}


void XmlElement::RemoveAttribute( const char * name )
{
	std::string str( name );
	XmlAttribute* node = attributeSet.Find( str );
	if ( node )
	{
		attributeSet.Remove( node );
		delete(node);
	}
}

const XmlElement* XmlNode::FirstChildElement() const
{
	const XmlNode* node;

	for (	node = FirstChild();
			node;
			node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


const XmlElement* XmlNode::FirstChildElement( const char * _value ) const
{
	const XmlNode* node;

	for (	node = FirstChild( _value );
			node;
			node = node->NextSibling( _value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


const XmlElement* XmlNode::NextSiblingElement() const
{
	const XmlNode* node;

	for (	node = NextSibling();
			node;
			node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


const XmlElement* XmlNode::NextSiblingElement( const char * _value ) const
{
	const XmlNode* node;

	for (	node = NextSibling( _value );
			node;
			node = node->NextSibling( _value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


const XmlDocument* XmlNode::GetDocument() const
{
	const XmlNode* node;

	for( node = this; node; node = node->parent )
	{
		if ( node->ToDocument() )
			return node->ToDocument();
	}
	return 0;
}


XmlElement::XmlElement ()
	: XmlNode( XmlNode::ELEMENT )
{
	firstChild = lastChild = 0;
}


XmlElement::XmlElement (const char * _value)
	: XmlNode( XmlNode::ELEMENT )
{
	firstChild = lastChild = 0;
	value = _value;
}


XmlElement::XmlElement( const std::string& _value ) 
	: XmlNode( XmlNode::ELEMENT )
{
	firstChild = lastChild = 0;
	value = _value;
}


XmlElement::XmlElement( const XmlElement& copy)
	: XmlNode( XmlNode::ELEMENT )
{
	firstChild = lastChild = 0;
	copy.CopyTo( this );	
}


void XmlElement::operator=( const XmlElement& base )
{
	ClearThis();
	base.CopyTo( this );
}


XmlElement::~XmlElement()
{
	ClearThis();
}


void XmlElement::ClearThis()
{
	Clear();
	while( attributeSet.First() )
	{
		XmlAttribute* node = attributeSet.First();
		attributeSet.Remove( node );
		delete(node);
	}
}


const char* XmlElement::Attribute( const char* name ) const
{
	const XmlAttribute* node = attributeSet.Find( name );
	if ( node )
		return node->Value();
	return 0;
}


const std::string* XmlElement::Attribute( const std::string& name ) const
{
	const XmlAttribute* node = attributeSet.Find( name );
	if ( node )
		return &node->ValueStr();
	return 0;
}


const char* XmlElement::Attribute( const char* name, int* i ) const
{
	const char* s = Attribute( name );
	if ( i )
	{
		if ( s ) {
			*i = atoi( s );
		}
		else {
			*i = 0;
		}
	}
	return s;
}


const std::string* XmlElement::Attribute( const std::string& name, int* i ) const
{
	const std::string* s = Attribute( name );
	if ( i )
	{
		if ( s ) {
			*i = atoi( s->c_str() );
		}
		else {
			*i = 0;
		}
	}
	return s;
}


const char* XmlElement::Attribute( const char* name, double* d ) const
{
	const char* s = Attribute( name );
	if ( d )
	{
		if ( s ) {
			*d = atof( s );
		}
		else {
			*d = 0;
		}
	}
	return s;
}


const std::string* XmlElement::Attribute( const std::string& name, double* d ) const
{
	const std::string* s = Attribute( name );
	if ( d )
	{
		if ( s ) {
			*d = atof( s->c_str() );
		}
		else {
			*d = 0;
		}
	}
	return s;
}


int XmlElement::QueryIntAttribute( const char* name, int* ival ) const
{
	const XmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return TIXML_NO_ATTRIBUTE;
	return node->QueryIntValue( ival );
}


int XmlElement::QueryIntAttribute( const std::string& name, int* ival ) const
{
	const XmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return TIXML_NO_ATTRIBUTE;
	return node->QueryIntValue( ival );
}


int XmlElement::QueryDoubleAttribute( const char* name, double* dval ) const
{
	const XmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return TIXML_NO_ATTRIBUTE;
	return node->QueryDoubleValue( dval );
}


int XmlElement::QueryDoubleAttribute( const std::string& name, double* dval ) const
{
	const XmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return TIXML_NO_ATTRIBUTE;
	return node->QueryDoubleValue( dval );
}


void XmlElement::SetAttribute( const char * name, int val )
{	
	char buf[64];
	#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF( buf, sizeof(buf), "%d", val );
	#else
		sprintf( buf, "%d", val );
	#endif
	SetAttribute( name, buf );
}


void XmlElement::SetAttribute( const std::string& name, int val )
{	
   std::ostringstream oss;
   oss << val;
   SetAttribute( name, oss.str() );
}


void XmlElement::SetDoubleAttribute( const char * name, double val )
{	
	char buf[256];
	#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF( buf, sizeof(buf), "%f", val );
	#else
		sprintf( buf, "%f", val );
	#endif
	SetAttribute( name, buf );
}


void XmlElement::SetAttribute( const char * cname, const char * cvalue )
{
	std::string _name( cname );
	std::string _value( cvalue );

	XmlAttribute* node = attributeSet.Find( _name );
	if ( node )
	{
		node->SetValue( _value );
		return;
	}

	XmlAttribute* attrib = new XmlAttribute( cname, cvalue );
	if ( attrib )
	{
		attributeSet.Add( attrib );
	}
	else
	{
		XmlDocument* document = GetDocument();
		if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, TIXML_ENCODING_UNKNOWN );
	}
}


void XmlElement::SetAttribute( const std::string& name, const std::string& _value )
{
	XmlAttribute* node = attributeSet.Find( name );
	if ( node )
	{
		node->SetValue( _value );
		return;
	}

	XmlAttribute* attrib = new XmlAttribute( name, _value );
	if ( attrib )
	{
		attributeSet.Add( attrib );
	}
	else
	{
		XmlDocument* document = GetDocument();
		if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, TIXML_ENCODING_UNKNOWN );
	}
}


void XmlElement::Print( FILE* cfile, int depth ) const
{
	int i;
	assert( cfile );
	for ( i=0; i<depth; i++ ) {
		fprintf( cfile, "    " );
	}

	fprintf( cfile, "<%s", value.c_str() );

	const XmlAttribute* attrib;
	for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
	{
		fprintf( cfile, " " );
		attrib->Print( cfile, depth );
	}

	// There are 3 different formatting approaches:
	// 1) An element without children is printed as a <foo /> node
	// 2) An element with only a text child is printed as <foo> text </foo>
	// 3) An element with children is printed on multiple lines.
	XmlNode* node;
	if ( !firstChild )
	{
		fprintf( cfile, " />" );
	}
	else if ( firstChild == lastChild && firstChild->ToText() )
	{
		fprintf( cfile, ">" );
		firstChild->Print( cfile, depth + 1 );
		fprintf( cfile, "</%s>", value.c_str() );
	}
	else
	{
		fprintf( cfile, ">" );

		for ( node = firstChild; node; node=node->NextSibling() )
		{
			if ( !node->ToText() )
			{
				fprintf( cfile, "\n" );
			}
			node->Print( cfile, depth+1 );
		}
		fprintf( cfile, "\n" );
		for( i=0; i<depth; ++i ) {
			fprintf( cfile, "    " );
		}
		fprintf( cfile, "</%s>", value.c_str() );
	}
}


void XmlElement::CopyTo( XmlElement* target ) const
{
	// superclass:
	XmlNode::CopyTo( target );

	// Element class: 
	// Clone the attributes, then clone the children.
	const XmlAttribute* attribute = 0;
	for(	attribute = attributeSet.First();
	attribute;
	attribute = attribute->Next() )
	{
		target->SetAttribute( attribute->Name(), attribute->Value() );
	}

	XmlNode* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		target->LinkEndChild( node->Clone() );
	}
}

bool XmlElement::Accept( XmlVisitor* visitor ) const
{
	if ( visitor->VisitEnter( *this, attributeSet.First() ) ) 
	{
		for ( const XmlNode* node=FirstChild(); node; node=node->NextSibling() )
		{
			if ( !node->Accept( visitor ) )
				break;
		}
	}
	return visitor->VisitExit( *this );
}


XmlNode* XmlElement::Clone() const
{
	XmlElement* clone = new XmlElement( Value() );
	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


const char* XmlElement::GetText() const
{
	const XmlNode* child = this->FirstChild();
	if ( child ) {
		const XmlText* childText = child->ToText();
		if ( childText ) {
			return childText->Value();
		}
	}
	return 0;
}


XmlDocument::XmlDocument() : XmlNode( XmlNode::DOCUMENT )
{
	tabsize = 4;
	useMicrosoftBOM = true;
	ClearError();
}

XmlDocument::XmlDocument( const char * documentName ) : XmlNode( XmlNode::DOCUMENT )
{
	tabsize = 4;
	useMicrosoftBOM = true;
	value = documentName;
	ClearError();
}


XmlDocument::XmlDocument( const std::string& documentName ) : XmlNode( XmlNode::DOCUMENT )
{
	tabsize = 4;
	useMicrosoftBOM = true;
    value = documentName;
	ClearError();
}


XmlDocument::XmlDocument( const XmlDocument& copy ) : XmlNode( XmlNode::DOCUMENT )
{
	copy.CopyTo( this );
}


void XmlDocument::operator=( const XmlDocument& copy )
{
	Clear();
	copy.CopyTo( this );
}

/*
bool XmlDocument::LoadFile( XmlEncoding encoding )
{
	// See STL_STRING_BUG below.
	//StringToBuffer buf( value );

	return LoadFile( Value(), encoding );
}
*/
/*
bool XmlDocument::SaveFile() const
{
	// See STL_STRING_BUG below.
//	StringToBuffer buf( value );
//
//	if ( buf.buffer && SaveFile( buf.buffer ) )
//		return true;
//
//	return false;
	return SaveFile( Value() );
}*/

bool XmlDocument::LoadFile(LPCSTR szVFSName, LPCTSTR szFilename, XmlEncoding encoding )
{
	// There was a really terrifying little bug here. The code:
	//		value = filename
	// in the STL case, cause the assignment method of the std::string to
	// be called. What is strange, is that the std::string had the same
	// address as it's c_str() method, and so bad things happen. Looks
	// like a bug in the Microsoft STL implementation.
	// Add an extra string to avoid the crash.
	//tstring filename( _filename );
	//value = filename;

	// reading in binary mode so that tinyxml can normalize the EOL
	/*FILE* file = XmlFOpen( _filename, _T("rb") );	
	if ( file )
	{
		bool result = LoadFile( file, encoding );
		fclose( file );
		return result;
	}
	else
	{
		SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
		return false;
	}*/

	DWORD dwTime = timeGetTime();

	if( szVFSName )
		m_pVFS = szVFSName;
	else
		m_pVFS = "VirtualFileSys";

	DWORD dwSize = m_pVFS->GetSize(szFilename);
	if( !GT_VALID(dwSize) )
	{
		SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
		return false;
	}
	if( 0 == dwSize )
	{
		SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return false;
	}

	// Delete the existing data:
	Clear();
	location.Clear();

	// If we have a file, assume it is all one big XML file, and read it in.
	// The document parser may decide the document ends sooner than the entire file, however.
	std::string data;
	data.reserve( dwSize );

	// Subtle bug here. TinyXml did use fgets. But from the XML spec:
	// 2.11 End-of-Line Handling
	// <snip>
	// <quote>
	// ...the XML processor MUST behave as if it normalized all line breaks in external 
	// parsed entities (including the document entity) on input, before parsing, by translating 
	// both the two-character sequence #xD #xA and any #xD that is not followed by #xA to 
	// a single #xA character.
	// </quote>
	//
	// It is not clear fgets does that, and certainly isn't clear it works cross platform. 
	// Generally, you expect fgets to translate from the convention of the OS to the c/unix
	// convention, and not work generally.

	/*
	while( fgets( buf, sizeof(buf), file ) )
	{
		data += buf;
	}
	*/

	char* buf = new char[ dwSize+1 ];
	buf[0] = 0;

	
	if( m_pVFS->LoadFile(buf, szFilename) != dwSize ) 
	{
		delete[](buf);
		SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
		return false;
	}

	const char* lastPos = buf;
	const char* p = buf;

	buf[dwSize] = 0;
	while( *p ) {
		assert( p < (buf+dwSize) );
		if ( *p == 0xa ) {
			// Newline character. No special rules for this. Append all the characters
			// since the last string, and include the newline.
			data.append( lastPos, (p-lastPos+1) );	// append, include the newline
			++p;									// move past the newline
			lastPos = p;							// and point to the new buffer (may be 0)
			assert( p <= (buf+dwSize) );
		}
		else if ( *p == 0xd ) {
			// Carriage return. Append what we have so far, then
			// handle moving forward in the buffer.
			if ( (p-lastPos) > 0 ) {
				data.append( lastPos, p-lastPos );	// do not add the CR
			}
			data += (char)0xa;						// a proper newline

			if ( *(p+1) == 0xa ) {
				// Carriage return - new line sequence
				p += 2;
				lastPos = p;
				assert( p <= (buf+dwSize) );
			}
			else {
				// it was followed by something else...that is presumably characters again.
				++p;
				lastPos = p;
				assert( p <= (buf+dwSize) );
			}
		}
		else {
			++p;
		}
	}
	// Handle any left over characters.
	if ( p-lastPos ) {
		data.append( lastPos, p-lastPos );
	}		
	delete[](buf);
	buf = 0;

	Parse( data.c_str(), 0, encoding );

	// IMSG(_T("tinyxml time=%d:%s\r\n"), timeGetTime()-dwTime, szFilename);

	if (  Error() )
        return false;
    else
		return true;
}


bool XmlDocument::SaveFile( const TCHAR * filename ) const
{
	// The old c stuff lives on...
	FILE* fp = XmlFOpen( filename, _T("w") );
	if ( fp )
	{
		bool result = SaveFile( fp );
		fclose( fp );
		return result;
	}
	return false;
}


bool XmlDocument::SaveFile( FILE* fp ) const
{
	if ( useMicrosoftBOM ) 
	{
		const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
		const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
		const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

		fputc( TIXML_UTF_LEAD_0, fp );
		fputc( TIXML_UTF_LEAD_1, fp );
		fputc( TIXML_UTF_LEAD_2, fp );
	}
	Print( fp, 0 );
	return (ferror(fp) == 0);
}


void XmlDocument::CopyTo( XmlDocument* target ) const
{
	XmlNode::CopyTo( target );

	target->error = error;
	target->errorId = errorId;
	target->errorDesc = errorDesc;
	target->tabsize = tabsize;
	target->errorLocation = errorLocation;
	target->useMicrosoftBOM = useMicrosoftBOM;

	XmlNode* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		target->LinkEndChild( node->Clone() );
	}	
}


XmlNode* XmlDocument::Clone() const
{
	XmlDocument* clone = new XmlDocument();
	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


void XmlDocument::Print( FILE* cfile, int depth ) const
{
	assert( cfile );
	for ( const XmlNode* node=FirstChild(); node; node=node->NextSibling() )
	{
		node->Print( cfile, depth );
		fprintf( cfile, "\n" );
	}
}


bool XmlDocument::Accept( XmlVisitor* visitor ) const
{
	if ( visitor->VisitEnter( *this ) )
	{
		for ( const XmlNode* node=FirstChild(); node; node=node->NextSibling() )
		{
			if ( !node->Accept( visitor ) )
				break;
		}
	}
	return visitor->VisitExit( *this );
}


const XmlAttribute* XmlAttribute::Next() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( next->value.empty() && next->name.empty() )
		return 0;
	return next;
}

/*
XmlAttribute* XmlAttribute::Next()
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( next->value.empty() && next->name.empty() )
		return 0;
	return next;
}
*/

const XmlAttribute* XmlAttribute::Previous() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( prev->value.empty() && prev->name.empty() )
		return 0;
	return prev;
}

/*
XmlAttribute* XmlAttribute::Previous()
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( prev->value.empty() && prev->name.empty() )
		return 0;
	return prev;
}
*/

void XmlAttribute::Print( FILE* cfile, int /*depth*/, std::string* str ) const
{
	std::string n, v;

	EncodeString( name, &n );
	EncodeString( value, &v );

	if (value.find ('\"') == std::string::npos) {
		if ( cfile ) {
		fprintf (cfile, "%s=\"%s\"", n.c_str(), v.c_str() );
		}
		if ( str ) {
			(*str) += n; (*str) += "=\""; (*str) += v; (*str) += "\"";
		}
	}
	else {
		if ( cfile ) {
		fprintf (cfile, "%s='%s'", n.c_str(), v.c_str() );
		}
		if ( str ) {
			(*str) += n; (*str) += "='"; (*str) += v; (*str) += "'";
		}
	}
}


int XmlAttribute::QueryIntValue( int* ival ) const
{
	if ( TIXML_SSCANF( value.c_str(), "%d", ival ) == 1 )
		return TIXML_SUCCESS;
	return TIXML_WRONG_TYPE;
}

int XmlAttribute::QueryDoubleValue( double* dval ) const
{
	if ( TIXML_SSCANF( value.c_str(), "%lf", dval ) == 1 )
		return TIXML_SUCCESS;
	return TIXML_WRONG_TYPE;
}

void XmlAttribute::SetIntValue( int _value )
{
	char buf [64];
	#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF(buf, sizeof(buf), "%d", _value);
	#else
		sprintf (buf, "%d", _value);
	#endif
	SetValue (buf);
}

void XmlAttribute::SetDoubleValue( double _value )
{
	char buf [256];
	#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF( buf, sizeof(buf), "%lf", _value);
	#else
		sprintf (buf, "%lf", _value);
	#endif
	SetValue (buf);
}

int XmlAttribute::IntValue() const
{
	return atoi (value.c_str ());
}

double  XmlAttribute::DoubleValue() const
{
	return atof (value.c_str ());
}


XmlComment::XmlComment( const XmlComment& copy ) : XmlNode( XmlNode::COMMENT )
{
	copy.CopyTo( this );
}


void XmlComment::operator=( const XmlComment& base )
{
	Clear();
	base.CopyTo( this );
}


void XmlComment::Print( FILE* cfile, int depth ) const
{
	assert( cfile );
	for ( int i=0; i<depth; i++ )
	{
		fprintf( cfile,  "    " );
	}
	fprintf( cfile, "<!--%s-->", value.c_str() );
}


void XmlComment::CopyTo( XmlComment* target ) const
{
	XmlNode::CopyTo( target );
}


bool XmlComment::Accept( XmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


XmlNode* XmlComment::Clone() const
{
	XmlComment* clone = new XmlComment();

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


void XmlText::Print( FILE* cfile, int depth ) const
{
	assert( cfile );
	if ( cdata )
	{
		int i;
		fprintf( cfile, "\n" );
		for ( i=0; i<depth; i++ ) {
			fprintf( cfile, "    " );
		}
		fprintf( cfile, "<![CDATA[%s]]>\n", value.c_str() );	// unformatted output
	}
	else
	{
		std::string buffer;
		EncodeString( value, &buffer );
		fprintf( cfile, "%s", buffer.c_str() );
	}
}


void XmlText::CopyTo( XmlText* target ) const
{
	XmlNode::CopyTo( target );
	target->cdata = cdata;
}


bool XmlText::Accept( XmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


XmlNode* XmlText::Clone() const
{	
	XmlText* clone = 0;
	clone = new XmlText( "" );

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


XmlDeclaration::XmlDeclaration( const char * _version,
									const char * _encoding,
									const char * _standalone )
	: XmlNode( XmlNode::DECLARATION )
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}


XmlDeclaration::XmlDeclaration(	const std::string& _version,
									const std::string& _encoding,
									const std::string& _standalone )
	: XmlNode( XmlNode::DECLARATION )
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}


XmlDeclaration::XmlDeclaration( const XmlDeclaration& copy )
	: XmlNode( XmlNode::DECLARATION )
{
	copy.CopyTo( this );	
}


void XmlDeclaration::operator=( const XmlDeclaration& copy )
{
	Clear();
	copy.CopyTo( this );
}


void XmlDeclaration::Print( FILE* cfile, int /*depth*/, std::string* str ) const
{
	if ( cfile ) fprintf( cfile, "<?xml " );
	if ( str )	 (*str) += "<?xml ";

	if ( !version.empty() ) {
		if ( cfile ) fprintf (cfile, "version=\"%s\" ", version.c_str ());
		if ( str ) { (*str) += "version=\""; (*str) += version; (*str) += "\" "; }
	}
	if ( !encoding.empty() ) {
		if ( cfile ) fprintf (cfile, "encoding=\"%s\" ", encoding.c_str ());
		if ( str ) { (*str) += "encoding=\""; (*str) += encoding; (*str) += "\" "; }
	}
	if ( !standalone.empty() ) {
		if ( cfile ) fprintf (cfile, "standalone=\"%s\" ", standalone.c_str ());
		if ( str ) { (*str) += "standalone=\""; (*str) += standalone; (*str) += "\" "; }
	}
	if ( cfile ) fprintf( cfile, "?>" );
	if ( str )	 (*str) += "?>";
}


void XmlDeclaration::CopyTo( XmlDeclaration* target ) const
{
	XmlNode::CopyTo( target );

	target->version = version;
	target->encoding = encoding;
	target->standalone = standalone;
}


bool XmlDeclaration::Accept( XmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


XmlNode* XmlDeclaration::Clone() const
{	
	XmlDeclaration* clone = new XmlDeclaration();

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


void XmlUnknown::Print( FILE* cfile, int depth ) const
{
	for ( int i=0; i<depth; i++ )
		fprintf( cfile, "    " );
	fprintf( cfile, "<%s>", value.c_str() );
}


void XmlUnknown::CopyTo( XmlUnknown* target ) const
{
	XmlNode::CopyTo( target );
}


bool XmlUnknown::Accept( XmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


XmlNode* XmlUnknown::Clone() const
{
	XmlUnknown* clone = new XmlUnknown();

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


XmlAttributeSet::XmlAttributeSet()
{
	sentinel.next = &sentinel;
	sentinel.prev = &sentinel;
}


XmlAttributeSet::~XmlAttributeSet()
{
	assert( sentinel.next == &sentinel );
	assert( sentinel.prev == &sentinel );
}


void XmlAttributeSet::Add( XmlAttribute* addMe )
{
	assert( !Find( std::string( addMe->Name() ) ) );	// Shouldn't be multiply adding to the set.
	addMe->next = &sentinel;
	addMe->prev = sentinel.prev;

	sentinel.prev->next = addMe;
	sentinel.prev      = addMe;
}

void XmlAttributeSet::Remove( XmlAttribute* removeMe )
{
	XmlAttribute* node;

	for( node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node == removeMe )
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->next = 0;
			node->prev = 0;
			return;
		}
	}
	assert( 0 );		// we tried to remove a non-linked attribute.
}



const XmlAttribute* XmlAttributeSet::Find( const std::string& name ) const
{
	for( const XmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node->name == name )
			return node;
	}
	return 0;
}

/*
XmlAttribute*	XmlAttributeSet::Find( const std::string& name )
{
	for( XmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node->name == name )
			return node;
	}
	return 0;
}
*/



const XmlAttribute* XmlAttributeSet::Find( const char* name ) const
{
	for( const XmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( strcmp( node->name.c_str(), name ) == 0 )
			return node;
	}
	return 0;
}

/*
XmlAttribute*	XmlAttributeSet::Find( const char* name )
{
	for( XmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( strcmp( node->name.c_str(), name ) == 0 )
			return node;
	}
	return 0;
}
*/

std::istream& operator>> (std::istream & in, XmlNode & base)
{
	std::string tag;
	tag.reserve( 8 * 1000 );
	base.StreamIn( &in, &tag );

	base.Parse( tag.c_str(), 0, TIXML_DEFAULT_ENCODING );
	return in;
}


std::ostream& operator<< (std::ostream & out, const XmlNode & base)
{
	XmlPrinter printer;
	printer.SetStreamPrinting();
	base.Accept( &printer );
	out << printer.Str();

	return out;
}


std::string& operator<< (std::string& out, const XmlNode& base )
{
	XmlPrinter printer;
	printer.SetStreamPrinting();
	base.Accept( &printer );
	out.append( printer.Str() );

	return out;
}

XmlHandle XmlHandle::FirstChild() const
{
	if ( node )
	{
		XmlNode* child = node->FirstChild();
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::FirstChild( const char * value ) const
{
	if ( node )
	{
		XmlNode* child = node->FirstChild( value );
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::FirstChildElement() const
{
	if ( node )
	{
		XmlElement* child = node->FirstChildElement();
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::FirstChildElement( const char * value ) const
{
	if ( node )
	{
		XmlElement* child = node->FirstChildElement( value );
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::Child( int count ) const
{
	if ( node )
	{
		int i;
		XmlNode* child = node->FirstChild();
		for (	i=0;
				child && i<count;
				child = child->NextSibling(), ++i )
		{
			// nothing
		}
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::Child( const char* value, int count ) const
{
	if ( node )
	{
		int i;
		XmlNode* child = node->FirstChild( value );
		for (	i=0;
				child && i<count;
				child = child->NextSibling( value ), ++i )
		{
			// nothing
		}
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::ChildElement( int count ) const
{
	if ( node )
	{
		int i;
		XmlElement* child = node->FirstChildElement();
		for (	i=0;
				child && i<count;
				child = child->NextSiblingElement(), ++i )
		{
			// nothing
		}
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::ChildElement( const char* value, int count ) const
{
	if ( node )
	{
		int i;
		XmlElement* child = node->FirstChildElement( value );
		for (	i=0;
				child && i<count;
				child = child->NextSiblingElement( value ), ++i )
		{
			// nothing
		}
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


bool XmlPrinter::VisitEnter( const XmlDocument& )
{
	return true;
}

bool XmlPrinter::VisitExit( const XmlDocument& )
{
	return true;
}

bool XmlPrinter::VisitEnter( const XmlElement& element, const XmlAttribute* firstAttribute )
{
	DoIndent();
	buffer += "<";
	buffer += element.Value();

	for( const XmlAttribute* attrib = firstAttribute; attrib; attrib = attrib->Next() )
	{
		buffer += " ";
		attrib->Print( 0, 0, &buffer );
	}

	if ( !element.FirstChild() ) 
	{
		buffer += " />";
		DoLineBreak();
	}
	else 
	{
		buffer += ">";
		if (    element.FirstChild()->ToText()
			  && element.LastChild() == element.FirstChild()
			  && element.FirstChild()->ToText()->CDATA() == false )
		{
			simpleTextPrint = true;
			// no DoLineBreak()!
		}
		else
		{
			DoLineBreak();
		}
	}
	++depth;	
	return true;
}


bool XmlPrinter::VisitExit( const XmlElement& element )
{
	--depth;
	if ( !element.FirstChild() ) 
	{
		// nothing.
	}
	else 
	{
		if ( simpleTextPrint )
		{
			simpleTextPrint = false;
		}
		else
		{
			DoIndent();
		}
		buffer += "</";
		buffer += element.Value();
		buffer += ">";
		DoLineBreak();
	}
	return true;
}


bool XmlPrinter::Visit( const XmlText& text )
{
	if ( text.CDATA() )
	{
		DoIndent();
		buffer += "<![CDATA[";
		buffer += text.Value();
		buffer += "]]>";
		DoLineBreak();
	}
	else if ( simpleTextPrint )
	{
		std::string str;
		XmlBase::EncodeString( text.ValueTStr(), &str );
		buffer += str;
	}
	else
	{
		DoIndent();
		std::string str;
		XmlBase::EncodeString( text.ValueTStr(), &str );
		buffer += str;
		DoLineBreak();
	}
	return true;
}


bool XmlPrinter::Visit( const XmlDeclaration& declaration )
{
	DoIndent();
	declaration.Print( 0, 0, &buffer );
	DoLineBreak();
	return true;
}


bool XmlPrinter::Visit( const XmlComment& comment )
{
	DoIndent();
	buffer += "<!--";
	buffer += comment.Value();
	buffer += "-->";
	DoLineBreak();
	return true;
}


bool XmlPrinter::Visit( const XmlUnknown& unknown )
{
	DoIndent();
	buffer += "<";
	buffer += unknown.Value();
	buffer += ">";
	DoLineBreak();
	return true;
}

}