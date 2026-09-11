#ifndef _ProxyPersistant_h_
#define _ProxyPersistant_h_

namespace rpn
{

template<class PROXY_CLASS>
class CProxyPersistant
{
	void SaveString(const QString& string, std::stringstream& stream)
	{
		std::string sName(string.toStdString());
		stream << '\"' << sName << "\" ";
	}
	QString LoadFromString(std::stringstream& stream)
	{
		// Read the value name
		std::string sName;

		// Search for the first "
		while(!stream.eof() && stream.get() != '\"')
      ;

    if(stream.eof())
      return QString();

		// Read string till next "
		char ch;
		do {
			ch = stream.get();
			if(ch != '\"')
				sName.append(1, ch);
		}while(ch != '\"');

		return sName.c_str();
	}
public:
	void LoadStream(CRpnStack& stack, std::stringstream& stream, CStreamVersion& version)
	{
		int nSize;
		stream >> nSize;
		for(int i = 0; i < nSize; i++)
		{
			QString sProxyId = LoadFromString(stream);
			PROXY_CLASS* pProxy = new PROXY_CLASS(stack, sProxyId);
			pProxy->LoadStream(stream, version);
		}
	}
	typedef std::vector<PROXY_CLASS*> TProxyVec;
	TProxyVec Proxies(CRpnStack& stack)
	{
		CRpnStack::TValueProxyVec vcProxy = stack.ValueProxies();
		TProxyVec vcRet;
		for(size_t i = 0; i < vcProxy.size(); i++)
		{
			PROXY_CLASS* pProxy = dynamic_cast<PROXY_CLASS*>(vcProxy[i]);
			if(pProxy) 
				vcRet.push_back(pProxy);
		}

		return vcRet;
	}
	void SaveStream(CRpnStack& stack, std::stringstream& stream)
	{
		// Collect proxies
		TProxyVec vcProxy = Proxies(stack);

		int nSize = vcProxy.size();
		stream << nSize << " ";
		for(size_t i = 0; i < vcProxy.size(); i++)
		{
			SaveString(vcProxy[i]->ProxyId().toStdString().c_str(), stream);
			vcProxy[i]->SaveStream(stream);
		}
	}
};

} // namespace rpn

#endif  // _ProxyPersistant_h_
