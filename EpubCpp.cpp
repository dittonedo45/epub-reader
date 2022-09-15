#include <iostream>
#include <regex>
#include <map>
extern "C" {
#include <archive.h>
#include <archive_entry.h>
};

using namespace std;

struct epub_handle {
	epub_handle (string filename)
	{
		init (filename);
	}
	~epub_handle ()
	{
		archive_read_free (ar);
	}


	void init (string& fn)
	{
		int r(0);
		ar=archive_read_new ();
		ent=archive_entry_new ();

		if (!ar)
			throw 1;

		try{
			r = archive_read_support_filter_all (ar);

			if (r)
				throw r;

			r = archive_read_support_format_all (ar);

			if (r)
				throw r;

			r = archive_read_open_filename (ar, fn.c_str (), 1054);

			if (r)
			{
				throw r;
			}
		}catch(int& error)
		{
			delete this;
			throw;
		}
	}
	struct archive_entry*
		next (map<string, string>& cont_b)
	{
		int r(archive_read_next_header (ar, &ent));
		if (r) throw r;
		string cont="";
		char buf[1054];

		while (1)
		{
			r=archive_read_data (ar, buf, 1054);
			if (!r) break;
			cont=cont+string (buf, r);
		}
		cont_b[string (archive_entry_pathname (ent))]=cont;
		return ent;
	}

	struct archive *ar;
	struct archive_entry *ent;
};

int main (int argsc, char **args)
{
	epub_handle* ep=new epub_handle(string (args[1]));
	map<string, string> cont_b;
	while (1)
	{
		try {
			struct archive_entry *ent=ep->next (cont_b);

			cout << archive_entry_pathname (ent) << endl;
		} catch (int& er)
		{
			break;
		}
	}
	regex opf("^.*\\.o..$");
	for (auto& cont_: cont_b)
	{
		if (!regex_match (cont_.first, opf))
			continue;
		cout << cont_.first << endl;
		cout << cont_.second << endl;
		break;
	}
	return( 0 );
}
