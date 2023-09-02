require "imap4flags";
require "regex";
if anyof (exists "X-Cron-Env",
          header :regex    ["subject"] [".* security run output",
                                        ".* monthly run output",
                                        ".* daily run output",
                                        ".* weekly run output"]) {
  addflag "$label1"; # ie 'Important'/red label within Thunderbird

# Other flags:
# addflag "$label1";  # Important: #ff0000 => red
# addflag "$label2";  # Work:      #ff9900 => orange
# addflag "$label3";  # personal:  #009900 => green
# addflag "$label4";  # todo:      #3333ff => blue
# addflag "$label5";  # later:     #993399 => violet
#
}