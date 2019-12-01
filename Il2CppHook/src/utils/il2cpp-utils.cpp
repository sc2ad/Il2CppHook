u16string_view csstrtostr(Il2CppString* in)
{
	return { in->chars, static_cast<uint32_t>(in->length) };
}

void setcsstr(Il2CppString* in, u16string_view str) {
	in->length = str.length();
	for (int i = 0; i < in->length; i++) {
		// Can assume that each char is only a single char (a single word --> double word)
		in->chars[i] = str[i];
	}
}