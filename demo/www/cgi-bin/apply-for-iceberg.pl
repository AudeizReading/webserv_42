#!/usr/bin/perl 

use POSIX;
# --- OU LES CHOSES SERIEUSES DOIVENT SE PASSER --------------------------------
if ($ENV{'REQUEST_METHOD'} eq "POST" ) 
{

	# l'upload doit se passer dans cette partie, on read les donnees puis on les envoie vers un fichier
	# check for upload file
	%_GET = &cgi_parse_request_string($ENV{'QUERY_STRING'});
	if ($ENV{'CONTENT_LENGTH'} > 0)
	{
		print "We are waiting for ".$ENV{'CONTENT_LENGTH'}." octets.\r\n";
		print "We are waiting for ".$ENV{'CONTENT_TYPE'}.".\r\n";

		binmode STDIN;

		$len_read = read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
		$len = length $buffer;
		print "<p> buffer lenght: $len</p>";
		print "<p> read lenght: $len_read</p>";
		#&cgi_print_ascii($buffer);
		#print "<p> buffer: [<pre>$buffer</pre>]</p>";

		# It seems that we do not read the same size of datas as expected, do not know why but we receive less than content length, so we do not have the full datas and the upload "failed" -> not really failed, but as we do not have the full datas, the file is created but the datas are corrupted (the new upload file can be seen in the gallery but as broken link, try to upload one file, you will be able to delete it from the gallery)
		#if (($len == $len_read && $len_read == $ENV{'CONTENT_LENGTH'}) || warn "We've read $len_read bytes but we are expecting $ENV{'CONTENT_LENGTH'}.")
		#{
		#}
		if (defined($ARGV[0]) || warn "It misses the boundary keys!") # Means that a boundary key is passed to the cgi script
		{
			&cgi_print_html_double_elt("p", "Another upload? <a href=\"../upload.html\">Click Here:</a>");
			$boundary = $ARGV[0];

			# also have to check is the upload size is acceptable or not
			if (($buffer =~ $boundary && $buffer =~ /(Content-Type\:\ image\/png|jpeg|jpg\ \n)/) || die "These datas are not allowed to be host on the server.")
			{
				my $buffer_len = length $buffer;
				&cgi_print_html_double_elt("p", "buffer de taille: $buffer_len");
				my %upload_files = &cgi_parse_body_upload($boundary, "$buffer");
				&cgi_upload_files(%upload_files);
				&cgi_print_html_double_elt("p", "End cgi_simulate_body_upload");

				&cgi_print_html_double_elt("p", "boundary ==> [$boundary]");
			}
		}
		$output_mess="STDIN (Methode POST)" ;
	}

	&cgi_print_html_dtd();
	&cgi_print_html_begin();
	&cgi_print_html_head();
	#&cgi_print_html_body_begin();
	#&cgi_print_html_double_elt("h1", "Résultat de la requete POST");
	#&cgi_print_html_double_elt("h2", $output_mess);
	#&cgi_print_html_double_elt("div", "<p>Raw Datas:</p> <pre>$buffer</pre>");
	#&cgi_print_html_double_elt("h2", "Liste des informations décodées");
	#print STDOUT "\t<ul>\r\n";
	#print "ARGV[0]\r\n";
	#&cgi_print_html_double_elt("li", $ARGV[0]);
	#print "GET\r\n";
	#&cgi_print_array_html(%_GET);
	#print "ENV\r\n";
	#&cgi_print_array_html(%ENV);
	#print STDOUT "\t</ul>\r\n";
	&cgi_print_html_double_elt("p", "Another upload? <a href=\"../upload.html\">Click Here:</a>");
	&cgi_print_html_double_elt("p", "Come back at index.html? <a href=\"../index.html\">Click Here:</a>");
	&cgi_print_html_body_end();
	&cgi_print_html_end();
}
elsif ($ENV{'REQUEST_METHOD'} eq "DELETE")
{
	%_GET = &cgi_parse_request_string($ENV{'QUERY_STRING'});
	if ($ENV{'CONTENT_LENGTH'} > 0)
	{
		read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
		%_POST=&cgi_parse_request_string($buffer);
	}
	if (defined($ENV{'PATH_INFO'}) || defined($_GET{'path_info'})) # Is there a path_info where searching datas ?
	{
		$directory = "..".$ENV{'PATH_INFO'};
		if (defined($_GET{'path_info'}))
		{
			$directory = "..".$_GET{'path_info'};
		}
		elsif (defined($_POST{'path_info'}))
		{
			$directory = "..".$_POST{'path_info'};
		}
	}

	# recuperer le nom du fichier a delete
	if (defined($_POST{'filename'})) # only 1 file
	{
		$file = $_POST{'filename'};
		#delete here
		if ( -e "../$file" and -f "../$file")
		{
			#delete here
			unlink("../$file") || warn "../$file couldn't be deleted: $!";
			&cgi_print_html_double_elt("p", "The file ../$file has been deleted"); 
		}
		else
		{
			die "You are trying to delete an element that is not on the server or that you are not allowed to access!";
		}
	}
	else # multi file
	{
		if (defined($_POST{'nb_files'}))
		{
			@_keys = grep(/filename/, (keys (%_POST)));
			foreach $file (@_keys)
			{
				if ( -e "$directory/$_POST{$file}" and -f "$directory/$_POST{$file}")
				{
					#delete here
					unlink("$directory/$_POST{$file}");
					&cgi_print_html_double_elt("p", "The file $directory/$_POST{$file} has been deleted"); 
				}
				else
				{
					die "You are trying to delete an element that is not on the server or that you are not allowed to access!";
				}
			}
		}
	}
}
elsif ($ENV{'REQUEST_METHOD'} eq "GET")
{
    $buffer = $ENV{'QUERY_STRING'};
	%_GET = &cgi_parse_request_string($ENV{'QUERY_STRING'});

	&cgi_print_html_dtd();
	&cgi_print_html_begin();
	&cgi_print_html_head();
	&cgi_print_html_body_begin();
	&cgi_print_html_double_elt("h1", "Our icebergs' collection");

	if (defined($ENV{'PATH_INFO'}) || defined($_GET{'path_info'})) # Is there a path_info where searching datas ?
	{
		$directory = "../".$ENV{'PATH_INFO'};
		#$directory = $ENV{'PATH_TRANSLATED'};
		if (defined($_GET{'path_info'}))
		{
			# ./demo/www/upload -> resultat obtenu, et suffisant pour opendir
			#$directory = $ENV{'PATH_TRANSLATED'}.$_GET{'path_info'};
			# avec le chdir du CGIManager, il faut aussi changer le path d'acces, nous sommes dans le rep cgi-bin
			$directory = "../".$_GET{'path_info'};
		}
		
		$absolute_path = $ENV{'PATH_TRANSLATED'}."cgi-bin/".$_GET{'path_info'};
		opendir(DIRECTORY_FD, $directory) || die "$directory couldn't be opened: $!";
		@FILES = grep(/\.png|jpe?g$/i, readdir DIRECTORY_FD);

		&cgi_display_files($_GET{'path_info'}, @FILES);
		closedir(DIRECTORY_FD);
	}
	&cgi_print_html_double_elt("p", "Come back at index.html? <a href=\"../index.html\">Click Here:</a>");
	&cgi_print_html_double_elt("p", "Book another iceberg? <a href=\"../upload.html\">Click Here:</a>");
	&cgi_print_html_body_end();
	&cgi_print_html_end();
}
else # other methods that we do not handle
{
	exit 1;
}
# --- OU LES CHOSES SERIEUSES DOIVENT FINIR DE SE PASSER -----------------------

sub	cgi_print_ascii
{
	my ($str) = @_;
	my ($len) = length $str;
	print "<p> convert $len ascii to int</br>";
	for(my $i=0; $i < $len; ++$i) 
	{
		my $ascii = substr($str, $i, 1);
		$ascii = ord($ascii);
		print "<span>==> $ascii <== </span>";
	}
	print "</p>";
}

# parse the body of an upload "POST" request
# cgi_parse_body_upload($boundary, $buffer_stdin);
sub	cgi_parse_body_upload
{
	my ($bound, $raw_datas) = @_;

	my (@body_datas) = split("--$bound", $raw_datas);
	my $body_datas_size = $#body_datas + 1;
	print "<p>cgi_parse_body_upload<br/>";
	print "bound: <span>$bound</span><br/>";
	print "body_datas_size: <span>$body_datas_size</span><br/>";
	#print "raw_datas: {<pre>$raw_datas</pre>}<br/>";
	print "</p>";

	# Les filename + les datas y correspondant sont dans body_datas[1] à body_datas[nb_upload]
	# body_datas[0] ne contiendrait que des \0 (je ne sais pas pq)
	# body_datas[$#body_datas - 1] contient les datas concernant l'input submit
	# body_datas[$#body_datas] contient --
	# taille minimale attendue si ce schema est respecté: 4
	if ($body_datas_size > 3)
	{
		my $nb_upload = $body_datas_size - 3;
		my (%files_data);

		for (my $i = 1; $i <= $nb_upload; ++$i)
		{
			my ($post_form_attributes, $file_datas, $remaining) = split(/\n\r\n/, $body_datas[$i]);
			my (@form_attr) = split(/[=;:"' ]/, $post_form_attributes);
			$files_data{$form_attr[11]} = $file_datas;
			print "<p>filename: $form_attr[11]<p/>";
			#&cgi_print_ascii($file_datas);
			#print "remaining: ";
			#&cgi_print_ascii($remaining);
		}
		return %files_data;
	}
	else
	{
		&cgi_print_html_double_elt("p", "There is a pb with the upload arguments");
	}
}

sub cgi_upload_files
{
	my (%upload_files) = @_;

	foreach $filename (keys %upload_files)
	{
		#print "<p>";
		#print "filename: <span>". $filename ."</span><br/>";
		#print "raw_datas: <pre>". $upload_files{$filename} ."</pre>";
		#print "</p>";

		open(UPLOAD_FILE, ">:raw", "../upload/$filename") || die "$filename couldn't be opened: $!";
		binmode UPLOAD_FILE;
		print UPLOAD_FILE $upload_files{$filename} || die "$filename couldn't be written: $!";
		close UPLOAD_FILE;
	}
}

# split by & and =, put this in a sort of map
# works with GET and also POST and ENV
sub cgi_parse_request_string
{
	my (@pairs, $pair, $name, $value, %form, $arg);

	($arg) = @_;
	@pairs = split(/&/, $arg);

	foreach $pair (@pairs) 
	{
		($name, $value) = split(/=/, $pair);
		$value =~ tr/+/ /;
		$value =~ s/%(..)/pack("C", hex($1))/eg;
		$form{$name} = $value;
	}
	%form;
}

# print any array nom
sub cgi_print_array_html
{
	local (%array) = @_;

	foreach $match (keys (%array)) {
		print STDOUT "\t\t<li><b>$match: </b>".$array{$match}."</li>\n";
	}
}

sub cgi_print_html_dtd
{
	print STDOUT "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n";
}

sub cgi_print_html_begin
{
	print STDOUT "<html lang=\"fr\">\r\n";
}

sub cgi_print_html_head
{
	print STDOUT "<head>\r\n";
	print STDOUT "\t<meta charset=\"UTF-8\">\r\n";
	print STDOUT "\t<title>$ENV{'REQUEST_METHOD'}</title>\r\n";
	print STDOUT "<link href=\"../main-style.css\" rel=\"stylesheet\" type=\"text/css\">"; # maybe the link could vary
	print STDOUT "<link href=\"http://fonts.cdnfonts.com/css/iceberg\" rel=\"stylesheet\">";
	print STDOUT "<script src=\"https://kit.fontawesome.com/dedab4d2ca.js\" crossorigin=\"anonymous\"></script>";
	print STDOUT "</head>\r\n";
}

sub cgi_print_html_body_begin
{
	print STDOUT "<body>\r\n";
}

sub cgi_print_html_end
{
	print STDOUT "</html>\r\n";
}

sub cgi_print_html_body_end
{
	print STDOUT "</body>\r\n";
}

sub cgi_print_html_double_elt
{
	my ($elt, $value) = @_;

	print STDOUT "<".$elt.">".$value."</".$elt.">\r\n";
}

sub cgi_print_html_input_submit_reset
{
	my ($value) = @_;

	print "<p>";
	print "<input type=\"submit\" name=\"submit\" value=\"$value\" />";
	#print "<input type=\"reset\" name=\"reset\" value=\"Reset\" />";
	print "</p>";
}

sub	cgi_response_header
{
	my ($code_response, $message_response) = @_;

	# Use a partial custom header functionnality
	print $ENV{'SERVER_PROTOCOL'}." ".$code_response." ".$message_response."\r\n";
	print "Content-Type: text/html\r\n";
	print "\r\n";



	#print $ENV{'SERVER_PROTOCOL'}." 200 OK\r\n"; # voir par la suite pour passer la rep en arg de la fn
	# la date ca peut fonctionner comme ca: soit on `date` et on formatte la sortie de la cmd
	#print "Date: ".`date`."\r\n"; # Pas le bon format
	# soit on utilise la facon perl (perldoc perfunc -> chercher localtime)
	# on est dans un contexte de liste:
	# ($sec, $min, $hour, $day_of_month, $month, $years_since_1900, $day_of_week, $day_of_year, $is_winter_time)=localtime;
	#print $day_of_month." / ".$month; # par exemple
	#print "Server: ".$ENV{'SERVER_SOFTWARE'}."\r\n";
	#print "Cache-Control: no-cache\r\n";
	#print "Content_length : ".$ENV{'CONTENT_LENGTH'}."\n"; # Pas obligatoire, vaut mieux rien mettre
}

sub	cgi_debug
{
	my ($need_env, %array) = @_;

	print STDOUT "<p>DEBUG<br/>\t<ul>\r\n";
	&cgi_print_array_html(%array);
	if ($need_env > 0)
	{
		&cgi_print_array_html(%ENV);
	}
	print STDOUT "\t</ul></p>\r\n";
}

sub cgi_delete_request_javascript
{
	my ($i, $path_info, $file) = @_;

	print "<script>\r\n";
	print "document.getElementById('delete-form$i').addEventListener('submit', function (event) {\r\n";
	print "		fetch(\"http://127.0.0.1:4242/$path_info/$file\", { method: 'DELETE' }).then(response => response.text()).then(() => {document.getElementById('output').textContent = \"Request sent\";location.reload();});\r\n";
	print "	event.preventDefault()\r\n";
	print "})\r\n";
	print "	</script>\r\n";

}

sub cgi_display_files
{
	my ($path_info, @files) = @_;
	my $i = 0;

	print STDOUT "\t<ul class=\"gallery\">\r\n";
	foreach $file (@files)
	{
		# ici mettre path /upload/$file pour que ca route derriere le cgi
		# faire un systeme de pagination si trop de photos
		&cgi_print_html_double_elt("li", "<img class=\"img_gallery\" src=\"$path_info/$file\"/>"); 

		print "<form class=\"delete-form\" id=\"delete-form$i\" method=\"DELETE\" action=\"#\" enctype=\"multipart/form-data\" alt=\"Deletion files(s)\">";
		#print "<form class=\"delete-form\" id=\"delete-form$i\" method=\"DELETE\" action=\"/cgi-bin/apply-for-iceberg.pl?/upload\" enctype=\"multipart/form-data\" alt=\"Deletion files(s)\">";
		&cgi_print_html_double_elt("p", "<input type=\"hidden\" name=\"path_info\" filename=\"$path_info/$file\" value=\"/upload\"/>"); 
		&cgi_print_html_input_submit_reset("Delete an Iceberg");

		# where the javascript sinppet displays its output
		print "<pre id=\"output\" style=\"max-height:300px;\">";
		print "</pre>";
		print "</form>";

		&cgi_delete_request_javascript($i, $path_info, $file);
		++$i;
	}
	print STDOUT "\t</ul>\r\n";
}
