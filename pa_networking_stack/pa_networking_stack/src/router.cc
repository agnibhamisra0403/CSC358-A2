#include "router.hh"

#include <iostream>
#include <limits>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
       << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
       << " on interface " << interface_num << "\n";

  routing_table.push_back(RouteStruct{route_prefix, prefix_length, next_hop, interface_num });
}

// read the destination address on every incoming packet and scan the rulebook to find most specific instruction for where to send it next.
void Router::route() {
  // loop through all the interfaces
  for (size_t i = 0; i < interfaces_.size(); i++) {
    auto datagram = interfaces_[i].maybe_receive();
    
    while (datagram.has_value()) {
      // get the destination IP
      InternetDatagram dg = datagram.value();
      uint32_t dst = dg.header().dst;

      int longest_prefix = -1;
      int best_match_index = -1;

      // loop through the routing table
      for (size_t j = 0; j < routing_table.size(); j++) {
        const RouteStruct& route = routing_table[j];
        
        // if prefix len is 0, then we cannot use shifting by 32 - avoided through the continue
        if (route.prefix_length == 0){
          if (route.prefix_length > longest_prefix) {
            longest_prefix = route.prefix_length;
            best_match_index = j;
          }
          continue;
        }

        uint32_t shifted_dst = dst >> (32 - route.prefix_length);
        uint32_t shifted_prefix = route.route_prefix >> (32 - route.prefix_length);
        
        // in the case that the top bits match check if this is a better match than the current match
        if (shifted_dst == shifted_route) {
          if (current.prefix_length > longest_prefix) {
            longest_prefix = current.prefix_length;
            best_match_index = j;
          }
        }
      }

      // check if we got a match
      if (best_match_index != -1) {
        // assign the best route to this match
        RouteStruct best_route = routing_table[best_match_index];

        // do phase 3 and 4 stuff here
      }

      else {
        // no match was found from the router table
      }

      datagram = interfaces_[i].maybe_receive();
    }
  }
}
