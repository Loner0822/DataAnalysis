#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "WebSocketpp.h"


websocketpp::connection_hdl connection_metadata::get_hdl() {
	return m_hdl;
}

void connection_metadata::on_open(client* c, websocketpp::connection_hdl hdl) {
	m_status = "Open";
	client::connection_ptr con = c->get_con_from_hdl(hdl);
	m_server = con->get_response_header("Server");
}

void connection_metadata::on_fail(client* c, websocketpp::connection_hdl hdl) {
	m_status = "Failed";
	client::connection_ptr con = c->get_con_from_hdl(hdl);
	m_server = con->get_response_header("Server");
	m_error_reason = con->get_ec().message();
}

void connection_metadata::on_close(client* c, websocketpp::connection_hdl hdl) {
	m_status = "Closed";
	client::connection_ptr con = c->get_con_from_hdl(hdl);
	std::stringstream s;
	s << "close code: " << con->get_remote_close_code() << " ("
		<< websocketpp::close::status::get_string(con->get_remote_close_code())
		<< "), close reason: " << con->get_remote_close_reason();
	m_error_reason = s.str();
}

int websocket_endpoint::connect(std::string const& uri) {
	websocketpp::lib::error_code ec;

	client::connection_ptr con = m_endpoint.get_connection(uri, ec);

	if (ec) {
		std::cout << "> Connect initialization error: " << ec.message() << std::endl;
		return -1;
	}

	int new_id = m_next_id++;
	connection_metadata::ptr metadata_ptr(new connection_metadata(new_id, con->get_handle(), uri));
	m_connection_list[new_id] = metadata_ptr;

	con->set_open_handler(websocketpp::lib::bind(
		&connection_metadata::on_open,
		metadata_ptr,
		&m_endpoint,
		websocketpp::lib::placeholders::_1
	));
	con->set_fail_handler(websocketpp::lib::bind(
		&connection_metadata::on_fail,
		metadata_ptr,
		&m_endpoint,
		websocketpp::lib::placeholders::_1
	));
	m_endpoint.connect(con);

	return new_id;
}

connection_metadata::ptr websocket_endpoint::get_metadata(int id) {
	con_list::const_iterator metadata_it = m_connection_list.find(id);
	if (metadata_it == m_connection_list.end()) {
		return connection_metadata::ptr();
	}
	else {
		return metadata_it->second;
	}
}

void websocket_endpoint::send(int id, std::string message) {
	websocketpp::lib::error_code ec;

	con_list::iterator metadata_it = m_connection_list.find(id);
	if (metadata_it == m_connection_list.end()) {
		std::cout << "> No connection found with id " << id << std::endl;
		return;
	}

	m_endpoint.send(metadata_it->second->get_hdl(), message, websocketpp::frame::opcode::text, ec);
	if (ec) {
		std::cout << "> Error sending message: " << ec.message() << std::endl;
		return;
	}

	//metadata_it->second->record_sent_message(message);
}
