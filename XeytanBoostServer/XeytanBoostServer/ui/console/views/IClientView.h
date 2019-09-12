#pragma once
#include <boost/smart_ptr/shared_ptr.hpp>
#include "IView.h"
#include "../../../models/Client.h"
#include "../../../Application.h"
#include <boost/array.hpp>


class IClientView : public IView
{
public:
	IClientView();
	IClientView(const boost::shared_ptr<ConsoleUiMediator>& mediator);
	virtual ~IClientView();

	void setActiveClient(const boost::shared_ptr<Client>& client);
;
protected:
	boost::shared_ptr<Client> client;
};
