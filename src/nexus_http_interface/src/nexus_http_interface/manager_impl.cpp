#include "manager_impl.hpp"
#include <spdlog/spdlog.h>
#include <cpr/cpr.h>

namespace nexuspool {
namespace nexus_http_interface {

Manager_impl::Manager_impl()
{}



void Manager_impl::pay_multiple() const
{

	
	
	//auto total_recipients = 99ull;
	//const auto max_payment_addresses = 99ull;
	//auto recipients_to_be_paid = total_recipients;
	//auto next_recipient_index = 0ull;
	//while (recipients_to_be_paid > 0)
	//{
	//	auto recipient_count_this_block = std::min(recipients_to_be_paid, max_payment_addresses);
	//	//make json string with addresses and amounts for up to 99 miners at a time.
	//	//post the api command to pay people.  the following is an example.
	//	cpr::Response r = cpr::Get(cpr::Url{ "https://api.github.com/repos/whoshuu/cpr/contributors" },
	//		cpr::Authentication{ "user", "pass" },
	//		cpr::Parameters{ {"anon", "true"}, {"key", "value"} });
	//	r.status_code;                  // 200
	//	r.header["content-type"];       // application/json; charset=utf-8
	//	r.text;                         // JSON text string
	//	//verify that it worked
	//	// update the database that the shares have been paid
	//	//do we need to wait 10 minutes between transactions to avoid fees?
	//	recipients_to_be_paid -= recipient_count_this_block;
	//}


}

}
}
