#!/bin/sh
#\
exec /usr/local/bin/tclsh8.6 "$0" "$@"

if 0 {
  switch -- $tcl_platform(platform) {
    unix {
      switch -- $tcl_platform(os) {
	Darwin {
	  load libtserialport1.0.dylib
	}
	default {
	  load ./libtserialport1.0.so
	}
      }
    }
    windows {
      load tserialport10.dll
    }
  }
} else {
  package require tserialport
}

set ports [tserialport::getports open]

puts "port count = [llength [dict keys $ports]]"

dict for {key data} $ports {
  puts "--- $key ---"
  foreach {name value} $data {
    puts "$name = $value"
  }
}

puts "--- device + description ---"
dict for {key data} $ports {
  puts "[dict get $data device] = [dict get $data description]"
}

puts "--- elatec ---"
set device {}
dict for {key data} $ports {
  if {[dict exists $data product]} {
    if {[string match TWN4* [dict get $data product]]} {
      set device [dict get $data device]
      break
    }
  }
}
puts TWN4=$device
