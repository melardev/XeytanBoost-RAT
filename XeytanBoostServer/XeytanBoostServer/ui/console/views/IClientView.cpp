#include "IClientView.h"



IClientView::IClientView()
{
}

IClientView::IClientView(const boost::shared_ptr<ConsoleUiMediator>& mediator):IView(mediator)
{
}


IClientView::~IClientView()
{
}

void IClientView::setActiveClient(const boost::shared_ptr<Client>& client)
{
	this->client = client;
}
