/* adpKliteGlobalMap.h - Map all klite global symbol */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------

01a,12oct07,san  created.
*/

#ifndef __INCadpKliteGlobalMapH

#define ERR_load_CRYPTO_strings                   ADP_ERR_load_CRYPTO_strings
#define CRYPTO_add_lock                           ADP_CRYPTO_add_lock
#define CRYPTO_destroy_dynlockid                  ADP_CRYPTO_destroy_dynlockid
#define CRYPTO_get_add_lock_callback              ADP_CRYPTO_get_add_lock_callback
#define CRYPTO_get_dynlock_create_callback        ADP_CRYPTO_get_dynlock_create_callback
#define CRYPTO_get_dynlock_destroy_callback       ADP_CRYPTO_get_dynlock_destroy_callback
#define CRYPTO_get_dynlock_lock_callback          ADP_CRYPTO_get_dynlock_lock_callback
#define CRYPTO_get_dynlock_value                  ADP_CRYPTO_get_dynlock_value
#define CRYPTO_get_id_callback                    ADP_CRYPTO_get_id_callback
#define CRYPTO_get_lock_name                      ADP_CRYPTO_get_lock_name
#define CRYPTO_get_locking_callback               ADP_CRYPTO_get_locking_callback
#define CRYPTO_get_new_dynlockid                  ADP_CRYPTO_get_new_dynlockid
#define CRYPTO_get_new_lockid                     ADP_CRYPTO_get_new_lockid
#define CRYPTO_lock                               ADP_CRYPTO_lock
#define CRYPTO_num_locks                          ADP_CRYPTO_num_locks
#define CRYPTO_set_add_lock_callback              ADP_CRYPTO_set_add_lock_callback
#define CRYPTO_set_dynlock_create_callback        ADP_CRYPTO_set_dynlock_create_callback
#define CRYPTO_set_dynlock_destroy_callback       ADP_CRYPTO_set_dynlock_destroy_callback
#define CRYPTO_set_dynlock_lock_callback          ADP_CRYPTO_set_dynlock_lock_callback
#define CRYPTO_set_id_callback                    ADP_CRYPTO_set_id_callback
#define CRYPTO_set_locking_callback               ADP_CRYPTO_set_locking_callback
#define CRYPTO_thread_id                          ADP_CRYPTO_thread_id
#define OPENSSL_NONPIC_relocated                  ADP_OPENSSL_NONPIC_relocated
#define OPENSSL_cpuid_setup                       ADP_OPENSSL_cpuid_setup
#define OPENSSL_ia32cap_loc                       ADP_OPENSSL_ia32cap_loc
#define OPENSSL_isservice                         ADP_OPENSSL_isservice
#define OPENSSL_showfatal                         ADP_OPENSSL_showfatal
#define OPENSSL_stderr                            ADP_OPENSSL_stderr
#define OpenSSLDie                                ADP_OpenSSLDie
#define SSLeay                                    ADP_SSLeay
#define SSLeay_version                            ADP_SSLeay_version
#define CRYPTO_cleanup_all_ex_data                ADP_CRYPTO_cleanup_all_ex_data
#define CRYPTO_dup_ex_data                        ADP_CRYPTO_dup_ex_data
#define CRYPTO_ex_data_new_class                  ADP_CRYPTO_ex_data_new_class
#define CRYPTO_free_ex_data                       ADP_CRYPTO_free_ex_data
#define CRYPTO_get_ex_data                        ADP_CRYPTO_get_ex_data
#define CRYPTO_get_ex_data_implementation         ADP_CRYPTO_get_ex_data_implementation
#define CRYPTO_get_ex_new_index                   ADP_CRYPTO_get_ex_new_index
#define CRYPTO_new_ex_data                        ADP_CRYPTO_new_ex_data
#define CRYPTO_set_ex_data                        ADP_CRYPTO_set_ex_data
#define CRYPTO_set_ex_data_implementation         ADP_CRYPTO_set_ex_data_implementation
#define CRYPTO_free                               ADP_CRYPTO_free
#define CRYPTO_free_locked                        ADP_CRYPTO_free_locked
#define CRYPTO_get_mem_functions                  ADP_CRYPTO_get_mem_functions
#define CRYPTO_malloc                             ADP_CRYPTO_malloc
#define CRYPTO_malloc_locked                      ADP_CRYPTO_malloc_locked
#define CRYPTO_realloc                            ADP_CRYPTO_realloc
#define CRYPTO_realloc_clean                      ADP_CRYPTO_realloc_clean
#define CRYPTO_set_mem_functions                  ADP_CRYPTO_set_mem_functions
#define OPENSSL_cleanse                           ADP_OPENSSL_cleanse
#define cleanse_ctr                               ADP_cleanse_ctr
#define CRYPTO_is_mem_check_on                    ADP_CRYPTO_is_mem_check_on
#define CRYPTO_mem_ctrl                           ADP_CRYPTO_mem_ctrl
#define CRYPTO_pop_info                           ADP_CRYPTO_pop_info
#define CRYPTO_push_info_                         ADP_CRYPTO_push_info_
#define OPENSSL_gmtime                            ADP_OPENSSL_gmtime
#define OPENSSL_issetugid                         ADP_OPENSSL_issetugid
#define AES_cbc_encrypt                           ADP_AES_cbc_encrypt
#define AES_cfb128_encrypt                        ADP_AES_cfb128_encrypt
#define AES_cfb1_encrypt                          ADP_AES_cfb1_encrypt
#define AES_cfb8_encrypt                          ADP_AES_cfb8_encrypt
#define AES_cfbr_encrypt_block                    ADP_AES_cfbr_encrypt_block
#define AES_decrypt                               ADP_AES_decrypt
#define AES_encrypt                               ADP_AES_encrypt
#define AES_set_decrypt_key                       ADP_AES_set_decrypt_key
#define AES_set_encrypt_key                       ADP_AES_set_encrypt_key
#define AES_ctr128_encrypt                        ADP_AES_ctr128_encrypt
#define AES_ecb_encrypt                           ADP_AES_ecb_encrypt
#define AES_options                               ADP_AES_options
#define AES_version                               ADP_AES_version
#define AES_ofb128_encrypt                        ADP_AES_ofb128_encrypt
#define AES_ccm128_decrypt                        ADP_AES_ccm128_decrypt
#define AES_ccm128_encrypt                        ADP_AES_ccm128_encrypt
#define AES_ccm128_test                           ADP_AES_ccm128_test
#define aes_ccmp_debug                            ADP_aes_ccmp_debug
#define AES_bi_ige_encrypt                        ADP_AES_bi_ige_encrypt
#define AES_ige_encrypt                           ADP_AES_ige_encrypt
#define ASN1_BIT_STRING_get_bit                   ADP_ASN1_BIT_STRING_get_bit
#define ASN1_BIT_STRING_set                       ADP_ASN1_BIT_STRING_set
#define ASN1_BIT_STRING_set_bit                   ADP_ASN1_BIT_STRING_set_bit
#define c2i_ASN1_BIT_STRING                       ADP_c2i_ASN1_BIT_STRING
#define i2c_ASN1_BIT_STRING                       ADP_i2c_ASN1_BIT_STRING
#define d2i_ASN1_BOOLEAN                          ADP_d2i_ASN1_BOOLEAN
#define i2d_ASN1_BOOLEAN                          ADP_i2d_ASN1_BOOLEAN
#define d2i_ASN1_bytes                            ADP_d2i_ASN1_bytes
#define d2i_ASN1_type_bytes                       ADP_d2i_ASN1_type_bytes
#define i2d_ASN1_bytes                            ADP_i2d_ASN1_bytes
#define ASN1_dup                                  ADP_ASN1_dup
#define ASN1_item_dup                             ADP_ASN1_item_dup
#define ASN1_HEADER_free                          ADP_ASN1_HEADER_free
#define ASN1_HEADER_new                           ADP_ASN1_HEADER_new
#define d2i_ASN1_HEADER                           ADP_d2i_ASN1_HEADER
#define i2d_ASN1_HEADER                           ADP_i2d_ASN1_HEADER
#define ASN1_INTEGER_cmp                          ADP_ASN1_INTEGER_cmp
#define ASN1_INTEGER_dup                          ADP_ASN1_INTEGER_dup
#define ASN1_INTEGER_get                          ADP_ASN1_INTEGER_get
#define ASN1_INTEGER_set                          ADP_ASN1_INTEGER_set
#define ASN1_INTEGER_to_BN                        ADP_ASN1_INTEGER_to_BN
#define BN_to_ASN1_INTEGER                        ADP_BN_to_ASN1_INTEGER
#define c2i_ASN1_INTEGER                          ADP_c2i_ASN1_INTEGER
#define d2i_ASN1_UINTEGER                         ADP_d2i_ASN1_UINTEGER
#define i2c_ASN1_INTEGER                          ADP_i2c_ASN1_INTEGER
#define ASN1_OBJECT_create                        ADP_ASN1_OBJECT_create
#define ASN1_OBJECT_free                          ADP_ASN1_OBJECT_free
#define ASN1_OBJECT_new                           ADP_ASN1_OBJECT_new
#define a2d_ASN1_OBJECT                           ADP_a2d_ASN1_OBJECT
#define c2i_ASN1_OBJECT                           ADP_c2i_ASN1_OBJECT
#define d2i_ASN1_OBJECT                           ADP_d2i_ASN1_OBJECT
#define i2a_ASN1_OBJECT                           ADP_i2a_ASN1_OBJECT
#define i2d_ASN1_OBJECT                           ADP_i2d_ASN1_OBJECT
#define i2t_ASN1_OBJECT                           ADP_i2t_ASN1_OBJECT
#define ASN1_OCTET_STRING_cmp                     ADP_ASN1_OCTET_STRING_cmp
#define ASN1_OCTET_STRING_dup                     ADP_ASN1_OCTET_STRING_dup
#define ASN1_OCTET_STRING_set                     ADP_ASN1_OCTET_STRING_set
#define d2i_ASN1_SET                              ADP_d2i_ASN1_SET
#define i2d_ASN1_SET                              ADP_i2d_ASN1_SET
#define ASN1_TYPE_get                             ADP_ASN1_TYPE_get
#define ASN1_TYPE_set                             ADP_ASN1_TYPE_set
#define ERR_load_ASN1_strings                     ADP_ERR_load_ASN1_strings
#define ASN1_STRING_cmp                           ADP_ASN1_STRING_cmp
#define ASN1_STRING_data                          ADP_ASN1_STRING_data
#define ASN1_STRING_dup                           ADP_ASN1_STRING_dup
#define ASN1_STRING_free                          ADP_ASN1_STRING_free
#define ASN1_STRING_length                        ADP_ASN1_STRING_length
#define ASN1_STRING_length_set                    ADP_ASN1_STRING_length_set
#define ASN1_STRING_new                           ADP_ASN1_STRING_new
#define ASN1_STRING_set                           ADP_ASN1_STRING_set
#define ASN1_STRING_type                          ADP_ASN1_STRING_type
#define ASN1_STRING_type_new                      ADP_ASN1_STRING_type_new
#define ASN1_check_infinite_end                   ADP_ASN1_check_infinite_end
#define ASN1_const_check_infinite_end             ADP_ASN1_const_check_infinite_end
#define ASN1_get_object                           ADP_ASN1_get_object
#define ASN1_object_size                          ADP_ASN1_object_size
#define ASN1_put_eoc                              ADP_ASN1_put_eoc
#define ASN1_put_object                           ADP_ASN1_put_object
#define ASN1_version                              ADP_ASN1_version
#define asn1_Finish                               ADP_asn1_Finish
#define asn1_GetSequence                          ADP_asn1_GetSequence
#define asn1_add_error                            ADP_asn1_add_error
#define asn1_const_Finish                         ADP_asn1_const_Finish
#define ASN1_parse                                ADP_ASN1_parse
#define ASN1_parse_dump                           ADP_ASN1_parse_dump
#define ASN1_tag2str                              ADP_ASN1_tag2str
#define ASN1_add_oid_module                       ADP_ASN1_add_oid_module
#define d2i_AutoPrivateKey                        ADP_d2i_AutoPrivateKey
#define d2i_PrivateKey                            ADP_d2i_PrivateKey
#define d2i_PublicKey                             ADP_d2i_PublicKey
#define ASN1_TYPE_get_int_octetstring             ADP_ASN1_TYPE_get_int_octetstring
#define ASN1_TYPE_get_octetstring                 ADP_ASN1_TYPE_get_octetstring
#define ASN1_TYPE_set_int_octetstring             ADP_ASN1_TYPE_set_int_octetstring
#define ASN1_TYPE_set_octetstring                 ADP_ASN1_TYPE_set_octetstring
#define a2i_ASN1_ENUMERATED                       ADP_a2i_ASN1_ENUMERATED
#define i2a_ASN1_ENUMERATED                       ADP_i2a_ASN1_ENUMERATED
#define a2i_ASN1_INTEGER                          ADP_a2i_ASN1_INTEGER
#define i2a_ASN1_INTEGER                          ADP_i2a_ASN1_INTEGER
#define a2i_ASN1_STRING                           ADP_a2i_ASN1_STRING
#define i2a_ASN1_STRING                           ADP_i2a_ASN1_STRING
#define i2d_PrivateKey                            ADP_i2d_PrivateKey
#define i2d_PublicKey                             ADP_i2d_PublicKey
#define NETSCAPE_ENCRYPTED_PKEY_free              ADP_NETSCAPE_ENCRYPTED_PKEY_free
#define NETSCAPE_ENCRYPTED_PKEY_it                ADP_NETSCAPE_ENCRYPTED_PKEY_it
#define NETSCAPE_ENCRYPTED_PKEY_new               ADP_NETSCAPE_ENCRYPTED_PKEY_new
#define NETSCAPE_PKEY_free                        ADP_NETSCAPE_PKEY_free
#define NETSCAPE_PKEY_it                          ADP_NETSCAPE_PKEY_it
#define NETSCAPE_PKEY_new                         ADP_NETSCAPE_PKEY_new
#define d2i_NETSCAPE_ENCRYPTED_PKEY               ADP_d2i_NETSCAPE_ENCRYPTED_PKEY
#define d2i_NETSCAPE_PKEY                         ADP_d2i_NETSCAPE_PKEY
#define d2i_Netscape_RSA                          ADP_d2i_Netscape_RSA
#define d2i_RSA_NET                               ADP_d2i_RSA_NET
#define i2d_NETSCAPE_ENCRYPTED_PKEY               ADP_i2d_NETSCAPE_ENCRYPTED_PKEY
#define i2d_NETSCAPE_PKEY                         ADP_i2d_NETSCAPE_PKEY
#define i2d_Netscape_RSA                          ADP_i2d_Netscape_RSA
#define i2d_RSA_NET                               ADP_i2d_RSA_NET
#define ASN1_item_d2i                             ADP_ASN1_item_d2i
#define ASN1_item_ex_d2i                          ADP_ASN1_item_ex_d2i
#define ASN1_tag2bit                              ADP_ASN1_tag2bit
#define ASN1_template_d2i                         ADP_ASN1_template_d2i
#define asn1_ex_c2i                               ADP_asn1_ex_c2i
#define ASN1_item_ex_i2d                          ADP_ASN1_item_ex_i2d
#define ASN1_item_i2d                             ADP_ASN1_item_i2d
#define ASN1_item_ndef_i2d                        ADP_ASN1_item_ndef_i2d
#define ASN1_template_i2d                         ADP_ASN1_template_i2d
#define asn1_ex_i2c                               ADP_asn1_ex_i2c
#define ASN1_item_ex_free                         ADP_ASN1_item_ex_free
#define ASN1_item_free                            ADP_ASN1_item_free
#define ASN1_primitive_free                       ADP_ASN1_primitive_free
#define ASN1_template_free                        ADP_ASN1_template_free
#define ASN1_item_ex_new                          ADP_ASN1_item_ex_new
#define ASN1_item_new                             ADP_ASN1_item_new
#define ASN1_primitive_new                        ADP_ASN1_primitive_new
#define ASN1_template_new                         ADP_ASN1_template_new
#define asn1_primitive_clear                      ADP_asn1_primitive_clear
#define ASN1_ANY_it                               ADP_ASN1_ANY_it
#define ASN1_BIT_STRING_free                      ADP_ASN1_BIT_STRING_free
#define ASN1_BIT_STRING_it                        ADP_ASN1_BIT_STRING_it
#define ASN1_BIT_STRING_new                       ADP_ASN1_BIT_STRING_new
#define ASN1_BMPSTRING_free                       ADP_ASN1_BMPSTRING_free
#define ASN1_BMPSTRING_it                         ADP_ASN1_BMPSTRING_it
#define ASN1_BMPSTRING_new                        ADP_ASN1_BMPSTRING_new
#define ASN1_BOOLEAN_it                           ADP_ASN1_BOOLEAN_it
#define ASN1_ENUMERATED_free                      ADP_ASN1_ENUMERATED_free
#define ASN1_ENUMERATED_it                        ADP_ASN1_ENUMERATED_it
#define ASN1_ENUMERATED_new                       ADP_ASN1_ENUMERATED_new
#define ASN1_FBOOLEAN_it                          ADP_ASN1_FBOOLEAN_it
#define ASN1_GENERALIZEDTIME_free                 ADP_ASN1_GENERALIZEDTIME_free
#define ASN1_GENERALIZEDTIME_it                   ADP_ASN1_GENERALIZEDTIME_it
#define ASN1_GENERALIZEDTIME_new                  ADP_ASN1_GENERALIZEDTIME_new
#define ASN1_GENERALSTRING_free                   ADP_ASN1_GENERALSTRING_free
#define ASN1_GENERALSTRING_it                     ADP_ASN1_GENERALSTRING_it
#define ASN1_GENERALSTRING_new                    ADP_ASN1_GENERALSTRING_new
#define ASN1_IA5STRING_free                       ADP_ASN1_IA5STRING_free
#define ASN1_IA5STRING_it                         ADP_ASN1_IA5STRING_it
#define ASN1_IA5STRING_new                        ADP_ASN1_IA5STRING_new
#define ASN1_INTEGER_free                         ADP_ASN1_INTEGER_free
#define ASN1_INTEGER_it                           ADP_ASN1_INTEGER_it
#define ASN1_INTEGER_new                          ADP_ASN1_INTEGER_new
#define ASN1_NULL_free                            ADP_ASN1_NULL_free
#define ASN1_NULL_it                              ADP_ASN1_NULL_it
#define ASN1_NULL_new                             ADP_ASN1_NULL_new
#define ASN1_OBJECT_it                            ADP_ASN1_OBJECT_it
#define ASN1_OCTET_STRING_NDEF_it                 ADP_ASN1_OCTET_STRING_NDEF_it
#define ASN1_OCTET_STRING_free                    ADP_ASN1_OCTET_STRING_free
#define ASN1_OCTET_STRING_it                      ADP_ASN1_OCTET_STRING_it
#define ASN1_OCTET_STRING_new                     ADP_ASN1_OCTET_STRING_new
#define ASN1_PRINTABLESTRING_free                 ADP_ASN1_PRINTABLESTRING_free
#define ASN1_PRINTABLESTRING_it                   ADP_ASN1_PRINTABLESTRING_it
#define ASN1_PRINTABLESTRING_new                  ADP_ASN1_PRINTABLESTRING_new
#define ASN1_PRINTABLE_free                       ADP_ASN1_PRINTABLE_free
#define ASN1_PRINTABLE_it                         ADP_ASN1_PRINTABLE_it
#define ASN1_PRINTABLE_new                        ADP_ASN1_PRINTABLE_new
#define ASN1_SEQUENCE_it                          ADP_ASN1_SEQUENCE_it
#define ASN1_T61STRING_free                       ADP_ASN1_T61STRING_free
#define ASN1_T61STRING_it                         ADP_ASN1_T61STRING_it
#define ASN1_T61STRING_new                        ADP_ASN1_T61STRING_new
#define ASN1_TBOOLEAN_it                          ADP_ASN1_TBOOLEAN_it
#define ASN1_TYPE_free                            ADP_ASN1_TYPE_free
#define ASN1_TYPE_new                             ADP_ASN1_TYPE_new
#define ASN1_UNIVERSALSTRING_free                 ADP_ASN1_UNIVERSALSTRING_free
#define ASN1_UNIVERSALSTRING_it                   ADP_ASN1_UNIVERSALSTRING_it
#define ASN1_UNIVERSALSTRING_new                  ADP_ASN1_UNIVERSALSTRING_new
#define ASN1_UTCTIME_free                         ADP_ASN1_UTCTIME_free
#define ASN1_UTCTIME_it                           ADP_ASN1_UTCTIME_it
#define ASN1_UTCTIME_new                          ADP_ASN1_UTCTIME_new
#define ASN1_UTF8STRING_free                      ADP_ASN1_UTF8STRING_free
#define ASN1_UTF8STRING_it                        ADP_ASN1_UTF8STRING_it
#define ASN1_UTF8STRING_new                       ADP_ASN1_UTF8STRING_new
#define ASN1_VISIBLESTRING_free                   ADP_ASN1_VISIBLESTRING_free
#define ASN1_VISIBLESTRING_it                     ADP_ASN1_VISIBLESTRING_it
#define ASN1_VISIBLESTRING_new                    ADP_ASN1_VISIBLESTRING_new
#define DIRECTORYSTRING_free                      ADP_DIRECTORYSTRING_free
#define DIRECTORYSTRING_it                        ADP_DIRECTORYSTRING_it
#define DIRECTORYSTRING_new                       ADP_DIRECTORYSTRING_new
#define DISPLAYTEXT_free                          ADP_DISPLAYTEXT_free
#define DISPLAYTEXT_it                            ADP_DISPLAYTEXT_it
#define DISPLAYTEXT_new                           ADP_DISPLAYTEXT_new
#define d2i_ASN1_BIT_STRING                       ADP_d2i_ASN1_BIT_STRING
#define d2i_ASN1_BMPSTRING                        ADP_d2i_ASN1_BMPSTRING
#define d2i_ASN1_ENUMERATED                       ADP_d2i_ASN1_ENUMERATED
#define d2i_ASN1_GENERALIZEDTIME                  ADP_d2i_ASN1_GENERALIZEDTIME
#define d2i_ASN1_GENERALSTRING                    ADP_d2i_ASN1_GENERALSTRING
#define d2i_ASN1_IA5STRING                        ADP_d2i_ASN1_IA5STRING
#define d2i_ASN1_INTEGER                          ADP_d2i_ASN1_INTEGER
#define d2i_ASN1_NULL                             ADP_d2i_ASN1_NULL
#define d2i_ASN1_OCTET_STRING                     ADP_d2i_ASN1_OCTET_STRING
#define d2i_ASN1_PRINTABLE                        ADP_d2i_ASN1_PRINTABLE
#define d2i_ASN1_PRINTABLESTRING                  ADP_d2i_ASN1_PRINTABLESTRING
#define d2i_ASN1_T61STRING                        ADP_d2i_ASN1_T61STRING
#define d2i_ASN1_TYPE                             ADP_d2i_ASN1_TYPE
#define d2i_ASN1_UNIVERSALSTRING                  ADP_d2i_ASN1_UNIVERSALSTRING
#define d2i_ASN1_UTCTIME                          ADP_d2i_ASN1_UTCTIME
#define d2i_ASN1_UTF8STRING                       ADP_d2i_ASN1_UTF8STRING
#define d2i_ASN1_VISIBLESTRING                    ADP_d2i_ASN1_VISIBLESTRING
#define d2i_DIRECTORYSTRING                       ADP_d2i_DIRECTORYSTRING
#define d2i_DISPLAYTEXT                           ADP_d2i_DISPLAYTEXT
#define i2d_ASN1_BIT_STRING                       ADP_i2d_ASN1_BIT_STRING
#define i2d_ASN1_BMPSTRING                        ADP_i2d_ASN1_BMPSTRING
#define i2d_ASN1_ENUMERATED                       ADP_i2d_ASN1_ENUMERATED
#define i2d_ASN1_GENERALIZEDTIME                  ADP_i2d_ASN1_GENERALIZEDTIME
#define i2d_ASN1_GENERALSTRING                    ADP_i2d_ASN1_GENERALSTRING
#define i2d_ASN1_IA5STRING                        ADP_i2d_ASN1_IA5STRING
#define i2d_ASN1_INTEGER                          ADP_i2d_ASN1_INTEGER
#define i2d_ASN1_NULL                             ADP_i2d_ASN1_NULL
#define i2d_ASN1_OCTET_STRING                     ADP_i2d_ASN1_OCTET_STRING
#define i2d_ASN1_PRINTABLE                        ADP_i2d_ASN1_PRINTABLE
#define i2d_ASN1_PRINTABLESTRING                  ADP_i2d_ASN1_PRINTABLESTRING
#define i2d_ASN1_T61STRING                        ADP_i2d_ASN1_T61STRING
#define i2d_ASN1_TYPE                             ADP_i2d_ASN1_TYPE
#define i2d_ASN1_UNIVERSALSTRING                  ADP_i2d_ASN1_UNIVERSALSTRING
#define i2d_ASN1_UTCTIME                          ADP_i2d_ASN1_UTCTIME
#define i2d_ASN1_UTF8STRING                       ADP_i2d_ASN1_UTF8STRING
#define i2d_ASN1_VISIBLESTRING                    ADP_i2d_ASN1_VISIBLESTRING
#define i2d_DIRECTORYSTRING                       ADP_i2d_DIRECTORYSTRING
#define i2d_DISPLAYTEXT                           ADP_i2d_DISPLAYTEXT
#define asn1_do_adb                               ADP_asn1_do_adb
#define asn1_do_lock                              ADP_asn1_do_lock
#define asn1_enc_free                             ADP_asn1_enc_free
#define asn1_enc_init                             ADP_asn1_enc_init
#define asn1_enc_restore                          ADP_asn1_enc_restore
#define asn1_enc_save                             ADP_asn1_enc_save
#define asn1_get_choice_selector                  ADP_asn1_get_choice_selector
#define asn1_get_field_ptr                        ADP_asn1_get_field_ptr
#define asn1_set_choice_selector                  ADP_asn1_set_choice_selector
#define DHparams_print                            ADP_DHparams_print
#define DHparams_print_fp                         ADP_DHparams_print_fp
#define DSA_print                                 ADP_DSA_print
#define DSA_print_fp                              ADP_DSA_print_fp
#define DSAparams_print                           ADP_DSAparams_print
#define DSAparams_print_fp                        ADP_DSAparams_print_fp
#define RSA_print                                 ADP_RSA_print
#define RSA_print_fp                              ADP_RSA_print_fp
#define X509_ALGOR_dup                            ADP_X509_ALGOR_dup
#define X509_ALGOR_free                           ADP_X509_ALGOR_free
#define X509_ALGOR_it                             ADP_X509_ALGOR_it
#define X509_ALGOR_new                            ADP_X509_ALGOR_new
#define d2i_X509_ALGOR                            ADP_d2i_X509_ALGOR
#define i2d_X509_ALGOR                            ADP_i2d_X509_ALGOR
#define X509_SIG_free                             ADP_X509_SIG_free
#define X509_SIG_it                               ADP_X509_SIG_it
#define X509_SIG_new                              ADP_X509_SIG_new
#define d2i_X509_SIG                              ADP_d2i_X509_SIG
#define i2d_X509_SIG                              ADP_i2d_X509_SIG
#define BIGNUM_it                                 ADP_BIGNUM_it
#define CBIGNUM_it                                ADP_CBIGNUM_it
#define LONG_it                                   ADP_LONG_it
#define ZLONG_it                                  ADP_ZLONG_it
#define BF_cfb64_encrypt                          ADP_BF_cfb64_encrypt
#define BF_ecb_encrypt                            ADP_BF_ecb_encrypt
#define BF_options                                ADP_BF_options
#define BF_version                                ADP_BF_version
#define BF_cbc_encrypt                            ADP_BF_cbc_encrypt
#define BF_decrypt                                ADP_BF_decrypt
#define BF_encrypt                                ADP_BF_encrypt
#define BF_ofb64_encrypt                          ADP_BF_ofb64_encrypt
#define BF_set_key                                ADP_BF_set_key
#define BIO_dump                                  ADP_BIO_dump
#define BIO_dump_cb                               ADP_BIO_dump_cb
#define BIO_dump_fp                               ADP_BIO_dump_fp
#define BIO_dump_indent                           ADP_BIO_dump_indent
#define BIO_dump_indent_cb                        ADP_BIO_dump_indent_cb
#define BIO_dump_indent_fp                        ADP_BIO_dump_indent_fp
#define BIO_printf                                ADP_BIO_printf
#define BIO_snprintf                              ADP_BIO_snprintf
#define BIO_vprintf                               ADP_BIO_vprintf
#define BIO_vsnprintf                             ADP_BIO_vsnprintf
#define BIO_accept                                ADP_BIO_accept
#define BIO_get_accept_socket                     ADP_BIO_get_accept_socket
#define BIO_get_host_ip                           ADP_BIO_get_host_ip
#define BIO_get_port                              ADP_BIO_get_port
#define BIO_gethostbyname                         ADP_BIO_gethostbyname
#define BIO_set_tcp_ndelay                        ADP_BIO_set_tcp_ndelay
#define BIO_sock_cleanup                          ADP_BIO_sock_cleanup
#define BIO_sock_error                            ADP_BIO_sock_error
#define BIO_sock_init                             ADP_BIO_sock_init
#define BIO_socket_ioctl                          ADP_BIO_socket_ioctl
#define BIO_socket_nbio                           ADP_BIO_socket_nbio
#define BIO_f_buffer                              ADP_BIO_f_buffer
#define BIO_f_linebuffer                          ADP_BIO_f_linebuffer
#define BIO_f_nbio_test                           ADP_BIO_f_nbio_test
#define BIO_f_null                                ADP_BIO_f_null
#define BIO_debug_callback                        ADP_BIO_debug_callback
#define ERR_load_BIO_strings                      ADP_ERR_load_BIO_strings
#define BIO_callback_ctrl                         ADP_BIO_callback_ctrl
#define BIO_clear_flags                           ADP_BIO_clear_flags
#define BIO_copy_next_retry                       ADP_BIO_copy_next_retry
#define BIO_ctrl                                  ADP_BIO_ctrl
#define BIO_ctrl_pending                          ADP_BIO_ctrl_pending
#define BIO_ctrl_wpending                         ADP_BIO_ctrl_wpending
#define BIO_dup_chain                             ADP_BIO_dup_chain
#define BIO_find_type                             ADP_BIO_find_type
#define BIO_free                                  ADP_BIO_free
#define BIO_free_all                              ADP_BIO_free_all
#define BIO_get_callback                          ADP_BIO_get_callback
#define BIO_get_callback_arg                      ADP_BIO_get_callback_arg
#define BIO_get_ex_data                           ADP_BIO_get_ex_data
#define BIO_get_ex_new_index                      ADP_BIO_get_ex_new_index
#define BIO_get_retry_BIO                         ADP_BIO_get_retry_BIO
#define BIO_get_retry_reason                      ADP_BIO_get_retry_reason
#define BIO_gets                                  ADP_BIO_gets
#define BIO_indent                                ADP_BIO_indent
#define BIO_int_ctrl                              ADP_BIO_int_ctrl
#define BIO_method_name                           ADP_BIO_method_name
#define BIO_method_type                           ADP_BIO_method_type
#define BIO_new                                   ADP_BIO_new
#define BIO_next                                  ADP_BIO_next
#define BIO_number_read                           ADP_BIO_number_read
#define BIO_number_written                        ADP_BIO_number_written
#define BIO_pop                                   ADP_BIO_pop
#define BIO_ptr_ctrl                              ADP_BIO_ptr_ctrl
#define BIO_push                                  ADP_BIO_push
#define BIO_puts                                  ADP_BIO_puts
#define BIO_read                                  ADP_BIO_read
#define BIO_set                                   ADP_BIO_set
#define BIO_set_callback                          ADP_BIO_set_callback
#define BIO_set_callback_arg                      ADP_BIO_set_callback_arg
#define BIO_set_ex_data                           ADP_BIO_set_ex_data
#define BIO_set_flags                             ADP_BIO_set_flags
#define BIO_test_flags                            ADP_BIO_test_flags
#define BIO_vfree                                 ADP_BIO_vfree
#define BIO_write                                 ADP_BIO_write
#define BIO_ACCEPT_free                           ADP_BIO_ACCEPT_free
#define BIO_ACCEPT_new                            ADP_BIO_ACCEPT_new
#define BIO_new_accept                            ADP_BIO_new_accept
#define BIO_s_accept                              ADP_BIO_s_accept
#define BIO_ctrl_get_read_request                 ADP_BIO_ctrl_get_read_request
#define BIO_ctrl_get_write_guarantee              ADP_BIO_ctrl_get_write_guarantee
#define BIO_ctrl_reset_read_request               ADP_BIO_ctrl_reset_read_request
#define BIO_new_bio_pair                          ADP_BIO_new_bio_pair
#define BIO_nread                                 ADP_BIO_nread
#define BIO_nread0                                ADP_BIO_nread0
#define BIO_nwrite                                ADP_BIO_nwrite
#define BIO_nwrite0                               ADP_BIO_nwrite0
#define BIO_s_bio                                 ADP_BIO_s_bio
#define BIO_CONNECT_free                          ADP_BIO_CONNECT_free
#define BIO_CONNECT_new                           ADP_BIO_CONNECT_new
#define BIO_new_connect                           ADP_BIO_new_connect
#define BIO_s_connect                             ADP_BIO_s_connect
#define BIO_fd_non_fatal_error                    ADP_BIO_fd_non_fatal_error
#define BIO_fd_should_retry                       ADP_BIO_fd_should_retry
#define BIO_new_fd                                ADP_BIO_new_fd
#define BIO_s_fd                                  ADP_BIO_s_fd
#define BIO_new_file                              ADP_BIO_new_file
#define BIO_new_fp                                ADP_BIO_new_fp
#define BIO_s_file                                ADP_BIO_s_file
#define BIO_s_log                                 ADP_BIO_s_log
#define BIO_new_mem_buf                           ADP_BIO_new_mem_buf
#define BIO_s_mem                                 ADP_BIO_s_mem
#define BIO_s_null                                ADP_BIO_s_null
#define BIO_new_socket                            ADP_BIO_new_socket
#define BIO_s_socket                              ADP_BIO_s_socket
#define BIO_sock_non_fatal_error                  ADP_BIO_sock_non_fatal_error
#define BIO_sock_should_retry                     ADP_BIO_sock_should_retry
#define BN_add                                    ADP_BN_add
#define BN_sub                                    ADP_BN_sub
#define BN_uadd                                   ADP_BN_uadd
#define BN_usub                                   ADP_BN_usub
#define bn_add_words                              ADP_bn_add_words
#define bn_div_words                              ADP_bn_div_words
#define bn_mul_add_words                          ADP_bn_mul_add_words
#define bn_mul_comba4                             ADP_bn_mul_comba4
#define bn_mul_comba8                             ADP_bn_mul_comba8
#define bn_mul_words                              ADP_bn_mul_words
#define bn_sqr_comba4                             ADP_bn_sqr_comba4
#define bn_sqr_comba8                             ADP_bn_sqr_comba8
#define bn_sqr_words                              ADP_bn_sqr_words
#define bn_sub_words                              ADP_bn_sub_words
#define BN_BLINDING_convert                       ADP_BN_BLINDING_convert
#define BN_BLINDING_convert_ex                    ADP_BN_BLINDING_convert_ex
#define BN_BLINDING_create_param                  ADP_BN_BLINDING_create_param
#define BN_BLINDING_free                          ADP_BN_BLINDING_free
#define BN_BLINDING_get_flags                     ADP_BN_BLINDING_get_flags
#define BN_BLINDING_get_thread_id                 ADP_BN_BLINDING_get_thread_id
#define BN_BLINDING_invert                        ADP_BN_BLINDING_invert
#define BN_BLINDING_invert_ex                     ADP_BN_BLINDING_invert_ex
#define BN_BLINDING_new                           ADP_BN_BLINDING_new
#define BN_BLINDING_set_flags                     ADP_BN_BLINDING_set_flags
#define BN_BLINDING_set_thread_id                 ADP_BN_BLINDING_set_thread_id
#define BN_BLINDING_update                        ADP_BN_BLINDING_update
#define BN_CTX_end                                ADP_BN_CTX_end
#define BN_CTX_free                               ADP_BN_CTX_free
#define BN_CTX_get                                ADP_BN_CTX_get
#define BN_CTX_init                               ADP_BN_CTX_init
#define BN_CTX_new                                ADP_BN_CTX_new
#define BN_CTX_start                              ADP_BN_CTX_start
#define BN_div                                    ADP_BN_div
#define ERR_load_BN_strings                       ADP_ERR_load_BN_strings
#define BN_exp                                    ADP_BN_exp
#define BN_mod_exp                                ADP_BN_mod_exp
#define BN_mod_exp_mont                           ADP_BN_mod_exp_mont
#define BN_mod_exp_mont_consttime                 ADP_BN_mod_exp_mont_consttime
#define BN_mod_exp_mont_word                      ADP_BN_mod_exp_mont_word
#define BN_mod_exp_recp                           ADP_BN_mod_exp_recp
#define BN_mod_exp_simple                         ADP_BN_mod_exp_simple
#define BN_mod_exp2_mont                          ADP_BN_mod_exp2_mont
#define BN_gcd                                    ADP_BN_gcd
#define BN_mod_inverse                            ADP_BN_mod_inverse
#define BN_bin2bn                                 ADP_BN_bin2bn
#define BN_bn2bin                                 ADP_BN_bn2bin
#define BN_clear                                  ADP_BN_clear
#define BN_clear_bit                              ADP_BN_clear_bit
#define BN_clear_free                             ADP_BN_clear_free
#define BN_cmp                                    ADP_BN_cmp
#define BN_copy                                   ADP_BN_copy
#define BN_dup                                    ADP_BN_dup
#define BN_free                                   ADP_BN_free
#define BN_get_params                             ADP_BN_get_params
#define BN_get_word                               ADP_BN_get_word
#define BN_init                                   ADP_BN_init
#define BN_is_bit_set                             ADP_BN_is_bit_set
#define BN_mask_bits                              ADP_BN_mask_bits
#define BN_new                                    ADP_BN_new
#define BN_num_bits                               ADP_BN_num_bits
#define BN_num_bits_word                          ADP_BN_num_bits_word
#define BN_options                                ADP_BN_options
#define BN_set_bit                                ADP_BN_set_bit
#define BN_set_negative                           ADP_BN_set_negative
#define BN_set_params                             ADP_BN_set_params
#define BN_set_word                               ADP_BN_set_word
#define BN_swap                                   ADP_BN_swap
#define BN_ucmp                                   ADP_BN_ucmp
#define BN_value_one                              ADP_BN_value_one
#define BN_version                                ADP_BN_version
#define bn_cmp_part_words                         ADP_bn_cmp_part_words
#define bn_cmp_words                              ADP_bn_cmp_words
#define bn_dup_expand                             ADP_bn_dup_expand
#define bn_expand2                                ADP_bn_expand2
#define BN_mod_add                                ADP_BN_mod_add
#define BN_mod_add_quick                          ADP_BN_mod_add_quick
#define BN_mod_lshift                             ADP_BN_mod_lshift
#define BN_mod_lshift1                            ADP_BN_mod_lshift1
#define BN_mod_lshift1_quick                      ADP_BN_mod_lshift1_quick
#define BN_mod_lshift_quick                       ADP_BN_mod_lshift_quick
#define BN_mod_mul                                ADP_BN_mod_mul
#define BN_mod_sqr                                ADP_BN_mod_sqr
#define BN_mod_sub                                ADP_BN_mod_sub
#define BN_mod_sub_quick                          ADP_BN_mod_sub_quick
#define BN_nnmod                                  ADP_BN_nnmod
#define BN_MONT_CTX_copy                          ADP_BN_MONT_CTX_copy
#define BN_MONT_CTX_free                          ADP_BN_MONT_CTX_free
#define BN_MONT_CTX_init                          ADP_BN_MONT_CTX_init
#define BN_MONT_CTX_new                           ADP_BN_MONT_CTX_new
#define BN_MONT_CTX_set                           ADP_BN_MONT_CTX_set
#define BN_MONT_CTX_set_locked                    ADP_BN_MONT_CTX_set_locked
#define BN_from_montgomery                        ADP_BN_from_montgomery
#define BN_mod_mul_montgomery                     ADP_BN_mod_mul_montgomery
#define BN_bn2mpi                                 ADP_BN_bn2mpi
#define BN_mpi2bn                                 ADP_BN_mpi2bn
#define BN_mul                                    ADP_BN_mul
#define bn_add_part_words                         ADP_bn_add_part_words
#define bn_mul_high                               ADP_bn_mul_high
#define bn_mul_low_normal                         ADP_bn_mul_low_normal
#define bn_mul_low_recursive                      ADP_bn_mul_low_recursive
#define bn_mul_normal                             ADP_bn_mul_normal
#define bn_mul_part_recursive                     ADP_bn_mul_part_recursive
#define bn_mul_recursive                          ADP_bn_mul_recursive
#define bn_sub_part_words                         ADP_bn_sub_part_words
#define BN_GENCB_call                             ADP_BN_GENCB_call
#define BN_generate_prime_ex                      ADP_BN_generate_prime_ex
#define BN_is_prime_ex                            ADP_BN_is_prime_ex
#define BN_is_prime_fasttest_ex                   ADP_BN_is_prime_fasttest_ex
#define BN_bn2dec                                 ADP_BN_bn2dec
#define BN_bn2hex                                 ADP_BN_bn2hex
#define BN_dec2bn                                 ADP_BN_dec2bn
#define BN_hex2bn                                 ADP_BN_hex2bn
#define BN_print                                  ADP_BN_print
#define BN_print_fp                               ADP_BN_print_fp
#define BN_bntest_rand                            ADP_BN_bntest_rand
#define BN_pseudo_rand                            ADP_BN_pseudo_rand
#define BN_pseudo_rand_range                      ADP_BN_pseudo_rand_range
#define BN_rand                                   ADP_BN_rand
#define BN_rand_range                             ADP_BN_rand_range
#define BN_RECP_CTX_free                          ADP_BN_RECP_CTX_free
#define BN_RECP_CTX_init                          ADP_BN_RECP_CTX_init
#define BN_RECP_CTX_new                           ADP_BN_RECP_CTX_new
#define BN_RECP_CTX_set                           ADP_BN_RECP_CTX_set
#define BN_div_recp                               ADP_BN_div_recp
#define BN_mod_mul_reciprocal                     ADP_BN_mod_mul_reciprocal
#define BN_reciprocal                             ADP_BN_reciprocal
#define BN_lshift                                 ADP_BN_lshift
#define BN_lshift1                                ADP_BN_lshift1
#define BN_rshift                                 ADP_BN_rshift
#define BN_rshift1                                ADP_BN_rshift1
#define BN_sqr                                    ADP_BN_sqr
#define bn_sqr_normal                             ADP_bn_sqr_normal
#define bn_sqr_recursive                          ADP_bn_sqr_recursive
#define BN_add_word                               ADP_BN_add_word
#define BN_div_word                               ADP_BN_div_word
#define BN_mod_word                               ADP_BN_mod_word
#define BN_mul_word                               ADP_BN_mul_word
#define BN_sub_word                               ADP_BN_sub_word
#define BN_mod_sqrt                               ADP_BN_mod_sqrt
#define BN_kronecker                              ADP_BN_kronecker
#define get_rfc2409_prime_1024                    ADP_get_rfc2409_prime_1024
#define get_rfc2409_prime_768                     ADP_get_rfc2409_prime_768
#define get_rfc3526_prime_1536                    ADP_get_rfc3526_prime_1536
#define get_rfc3526_prime_2048                    ADP_get_rfc3526_prime_2048
#define get_rfc3526_prime_3072                    ADP_get_rfc3526_prime_3072
#define get_rfc3526_prime_4096                    ADP_get_rfc3526_prime_4096
#define get_rfc3526_prime_6144                    ADP_get_rfc3526_prime_6144
#define get_rfc3526_prime_8192                    ADP_get_rfc3526_prime_8192
#define BN_GF2m_add                               ADP_BN_GF2m_add
#define BN_GF2m_arr2poly                          ADP_BN_GF2m_arr2poly
#define BN_GF2m_mod                               ADP_BN_GF2m_mod
#define BN_GF2m_mod_arr                           ADP_BN_GF2m_mod_arr
#define BN_GF2m_mod_div                           ADP_BN_GF2m_mod_div
#define BN_GF2m_mod_div_arr                       ADP_BN_GF2m_mod_div_arr
#define BN_GF2m_mod_exp                           ADP_BN_GF2m_mod_exp
#define BN_GF2m_mod_exp_arr                       ADP_BN_GF2m_mod_exp_arr
#define BN_GF2m_mod_inv                           ADP_BN_GF2m_mod_inv
#define BN_GF2m_mod_inv_arr                       ADP_BN_GF2m_mod_inv_arr
#define BN_GF2m_mod_mul                           ADP_BN_GF2m_mod_mul
#define BN_GF2m_mod_mul_arr                       ADP_BN_GF2m_mod_mul_arr
#define BN_GF2m_mod_solve_quad                    ADP_BN_GF2m_mod_solve_quad
#define BN_GF2m_mod_solve_quad_arr                ADP_BN_GF2m_mod_solve_quad_arr
#define BN_GF2m_mod_sqr                           ADP_BN_GF2m_mod_sqr
#define BN_GF2m_mod_sqr_arr                       ADP_BN_GF2m_mod_sqr_arr
#define BN_GF2m_mod_sqrt                          ADP_BN_GF2m_mod_sqrt
#define BN_GF2m_mod_sqrt_arr                      ADP_BN_GF2m_mod_sqrt_arr
#define BN_GF2m_poly2arr                          ADP_BN_GF2m_poly2arr
#define BN_get0_nist_prime_192                    ADP_BN_get0_nist_prime_192
#define BN_get0_nist_prime_224                    ADP_BN_get0_nist_prime_224
#define BN_get0_nist_prime_256                    ADP_BN_get0_nist_prime_256
#define BN_get0_nist_prime_384                    ADP_BN_get0_nist_prime_384
#define BN_get0_nist_prime_521                    ADP_BN_get0_nist_prime_521
#define BN_nist_mod_192                           ADP_BN_nist_mod_192
#define BN_nist_mod_224                           ADP_BN_nist_mod_224
#define BN_nist_mod_256                           ADP_BN_nist_mod_256
#define BN_nist_mod_384                           ADP_BN_nist_mod_384
#define BN_nist_mod_521                           ADP_BN_nist_mod_521
#define BN_generate_prime                         ADP_BN_generate_prime
#define BN_is_prime                               ADP_BN_is_prime
#define BN_is_prime_fasttest                      ADP_BN_is_prime_fasttest
#define BUF_MEM_free                              ADP_BUF_MEM_free
#define BUF_MEM_grow                              ADP_BUF_MEM_grow
#define BUF_MEM_grow_clean                        ADP_BUF_MEM_grow_clean
#define BUF_MEM_new                               ADP_BUF_MEM_new
#define BUF_memdup                                ADP_BUF_memdup
#define BUF_strdup                                ADP_BUF_strdup
#define BUF_strlcat                               ADP_BUF_strlcat
#define BUF_strlcpy                               ADP_BUF_strlcpy
#define BUF_strndup                               ADP_BUF_strndup
#define ERR_load_BUF_strings                      ADP_ERR_load_BUF_strings
#define CAST_cfb64_encrypt                        ADP_CAST_cfb64_encrypt
#define CAST_ecb_encrypt                          ADP_CAST_ecb_encrypt
#define CAST_version                              ADP_CAST_version
#define CAST_cbc_encrypt                          ADP_CAST_cbc_encrypt
#define CAST_decrypt                              ADP_CAST_decrypt
#define CAST_encrypt                              ADP_CAST_encrypt
#define CAST_ofb64_encrypt                        ADP_CAST_ofb64_encrypt
#define CAST_S_table0                             ADP_CAST_S_table0
#define CAST_S_table1                             ADP_CAST_S_table1
#define CAST_S_table2                             ADP_CAST_S_table2
#define CAST_S_table3                             ADP_CAST_S_table3
#define CAST_S_table4                             ADP_CAST_S_table4
#define CAST_S_table5                             ADP_CAST_S_table5
#define CAST_S_table6                             ADP_CAST_S_table6
#define CAST_S_table7                             ADP_CAST_S_table7
#define CAST_set_key                              ADP_CAST_set_key
#define COMP_CTX_free                             ADP_COMP_CTX_free
#define COMP_CTX_new                              ADP_COMP_CTX_new
#define COMP_compress_block                       ADP_COMP_compress_block
#define COMP_expand_block                         ADP_COMP_expand_block
#define COMP_rle                                  ADP_COMP_rle
#define COMP_zlib                                 ADP_COMP_zlib
#define _CONF_add_string                          ADP__CONF_add_string
#define _CONF_free_data                           ADP__CONF_free_data
#define _CONF_get_section                         ADP__CONF_get_section
#define _CONF_get_section_values                  ADP__CONF_get_section_values
#define _CONF_get_string                          ADP__CONF_get_string
#define _CONF_new_data                            ADP__CONF_new_data
#define _CONF_new_section                         ADP__CONF_new_section
#define CONF_def_version                          ADP_CONF_def_version
#define NCONF_WIN32                               ADP_NCONF_WIN32
#define NCONF_default                             ADP_NCONF_default
#define ERR_load_CONF_strings                     ADP_ERR_load_CONF_strings
#define OPENSSL_load_builtin_modules              ADP_OPENSSL_load_builtin_modules
#define CONF_dump_bio                             ADP_CONF_dump_bio
#define CONF_dump_fp                              ADP_CONF_dump_fp
#define CONF_free                                 ADP_CONF_free
#define CONF_get_number                           ADP_CONF_get_number
#define CONF_get_section                          ADP_CONF_get_section
#define CONF_get_string                           ADP_CONF_get_string
#define CONF_load                                 ADP_CONF_load
#define CONF_load_bio                             ADP_CONF_load_bio
#define CONF_load_fp                              ADP_CONF_load_fp
#define CONF_set_default_method                   ADP_CONF_set_default_method
#define CONF_set_nconf                            ADP_CONF_set_nconf
#define CONF_version                              ADP_CONF_version
#define NCONF_dump_bio                            ADP_NCONF_dump_bio
#define NCONF_dump_fp                             ADP_NCONF_dump_fp
#define NCONF_free                                ADP_NCONF_free
#define NCONF_free_data                           ADP_NCONF_free_data
#define NCONF_get_number_e                        ADP_NCONF_get_number_e
#define NCONF_get_section                         ADP_NCONF_get_section
#define NCONF_get_string                          ADP_NCONF_get_string
#define NCONF_load                                ADP_NCONF_load
#define NCONF_load_bio                            ADP_NCONF_load_bio
#define NCONF_load_fp                             ADP_NCONF_load_fp
#define NCONF_new                                 ADP_NCONF_new
#define CONF_get1_default_config_file             ADP_CONF_get1_default_config_file
#define CONF_imodule_get_flags                    ADP_CONF_imodule_get_flags
#define CONF_imodule_get_module                   ADP_CONF_imodule_get_module
#define CONF_imodule_get_name                     ADP_CONF_imodule_get_name
#define CONF_imodule_get_usr_data                 ADP_CONF_imodule_get_usr_data
#define CONF_imodule_get_value                    ADP_CONF_imodule_get_value
#define CONF_imodule_set_flags                    ADP_CONF_imodule_set_flags
#define CONF_imodule_set_usr_data                 ADP_CONF_imodule_set_usr_data
#define CONF_module_add                           ADP_CONF_module_add
#define CONF_module_get_usr_data                  ADP_CONF_module_get_usr_data
#define CONF_module_set_usr_data                  ADP_CONF_module_set_usr_data
#define CONF_modules_finish                       ADP_CONF_modules_finish
#define CONF_modules_free                         ADP_CONF_modules_free
#define CONF_modules_load                         ADP_CONF_modules_load
#define CONF_modules_load_file                    ADP_CONF_modules_load_file
#define CONF_modules_unload                       ADP_CONF_modules_unload
#define CONF_parse_list                           ADP_CONF_parse_list
#define OPENSSL_config                            ADP_OPENSSL_config
#define OPENSSL_no_config                         ADP_OPENSSL_no_config
#define DES_cbc_cksum                             ADP_DES_cbc_cksum
#define DES_cbc_encrypt                           ADP_DES_cbc_encrypt
#define DES_ede3_cfb64_encrypt                    ADP_DES_ede3_cfb64_encrypt
#define DES_ede3_cfb_encrypt                      ADP_DES_ede3_cfb_encrypt
#define DES_cfb64_encrypt                         ADP_DES_cfb64_encrypt
#define DES_cfb_encrypt                           ADP_DES_cfb_encrypt
#define DES_decrypt3                              ADP_DES_decrypt3
#define DES_ede3_cbc_encrypt                      ADP_DES_ede3_cbc_encrypt
#define DES_encrypt1                              ADP_DES_encrypt1
#define DES_encrypt2                              ADP_DES_encrypt2
#define DES_encrypt3                              ADP_DES_encrypt3
#define DES_ncbc_encrypt                          ADP_DES_ncbc_encrypt
#define DES_ecb3_encrypt                          ADP_DES_ecb3_encrypt
#define DES_SPtrans                               ADP_DES_SPtrans
#define DES_ecb_encrypt                           ADP_DES_ecb_encrypt
#define DES_options                               ADP_DES_options
#define OSSL_DES_version                          ADP_OSSL_DES_version
#define OSSL_libdes_version                       ADP_OSSL_libdes_version
#define DES_enc_read                              ADP_DES_enc_read
#define _shadow_DES_rw_mode                       ADP__shadow_DES_rw_mode
#define DES_enc_write                             ADP_DES_enc_write
#define DES_crypt                                 ADP_DES_crypt
#define DES_fcrypt                                ADP_DES_fcrypt
#define fcrypt_body                               ADP_fcrypt_body
#define DES_ede3_ofb64_encrypt                    ADP_DES_ede3_ofb64_encrypt
#define DES_ofb64_encrypt                         ADP_DES_ofb64_encrypt
#define DES_ofb_encrypt                           ADP_DES_ofb_encrypt
#define DES_pcbc_encrypt                          ADP_DES_pcbc_encrypt
#define DES_quad_cksum                            ADP_DES_quad_cksum
#define DES_random_key                            ADP_DES_random_key
#define DES_read_2passwords                       ADP_DES_read_2passwords
#define DES_read_password                         ADP_DES_read_password
#define _des_crypt                                ADP__des_crypt
#define DES_check_key_parity                      ADP_DES_check_key_parity
#define DES_is_weak_key                           ADP_DES_is_weak_key
#define DES_key_sched                             ADP_DES_key_sched
#define DES_set_key                               ADP_DES_set_key
#define DES_set_key_checked                       ADP_DES_set_key_checked
#define DES_set_key_unchecked                     ADP_DES_set_key_unchecked
#define DES_set_odd_parity                        ADP_DES_set_odd_parity
#define _shadow_DES_check_key                     ADP__shadow_DES_check_key
#define DES_string_to_2keys                       ADP_DES_string_to_2keys
#define DES_string_to_key                         ADP_DES_string_to_key
#define DES_xcbc_encrypt                          ADP_DES_xcbc_encrypt
#define DES_xwhite_in2out                         ADP_DES_xwhite_in2out
#define DHparams_it                               ADP_DHparams_it
#define d2i_DHparams                              ADP_d2i_DHparams
#define i2d_DHparams                              ADP_i2d_DHparams
#define DH_check                                  ADP_DH_check
#define DH_check_pub_key                          ADP_DH_check_pub_key
#define ERR_load_DH_strings                       ADP_ERR_load_DH_strings
#define DH_generate_parameters_ex                 ADP_DH_generate_parameters_ex
#define DH_OpenSSL                                ADP_DH_OpenSSL
#define DH_compute_key                            ADP_DH_compute_key
#define DH_generate_key                           ADP_DH_generate_key
#define DH_free                                   ADP_DH_free
#define DH_get_default_method                     ADP_DH_get_default_method
#define DH_get_ex_data                            ADP_DH_get_ex_data
#define DH_get_ex_new_index                       ADP_DH_get_ex_new_index
#define DH_new                                    ADP_DH_new
#define DH_new_method                             ADP_DH_new_method
#define DH_set_default_method                     ADP_DH_set_default_method
#define DH_set_ex_data                            ADP_DH_set_ex_data
#define DH_set_method                             ADP_DH_set_method
#define DH_size                                   ADP_DH_size
#define DH_up_ref                                 ADP_DH_up_ref
#define DH_version                                ADP_DH_version
#define DH_generate_parameters                    ADP_DH_generate_parameters
#define DSAPrivateKey_it                          ADP_DSAPrivateKey_it
#define DSAPublicKey_it                           ADP_DSAPublicKey_it
#define DSA_SIG_free                              ADP_DSA_SIG_free
#define DSA_SIG_it                                ADP_DSA_SIG_it
#define DSA_SIG_new                               ADP_DSA_SIG_new
#define DSAparams_it                              ADP_DSAparams_it
#define d2i_DSAPrivateKey                         ADP_d2i_DSAPrivateKey
#define d2i_DSAPublicKey                          ADP_d2i_DSAPublicKey
#define d2i_DSA_SIG                               ADP_d2i_DSA_SIG
#define d2i_DSAparams                             ADP_d2i_DSAparams
#define dsa_pub_internal_it                       ADP_dsa_pub_internal_it
#define i2d_DSAPrivateKey                         ADP_i2d_DSAPrivateKey
#define i2d_DSAPublicKey                          ADP_i2d_DSAPublicKey
#define i2d_DSA_SIG                               ADP_i2d_DSA_SIG
#define i2d_DSAparams                             ADP_i2d_DSAparams
#define ERR_load_DSA_strings                      ADP_ERR_load_DSA_strings
#define DSA_generate_parameters_ex                ADP_DSA_generate_parameters_ex
#define DSA_generate_key                          ADP_DSA_generate_key
#define DSA_dup_DH                                ADP_DSA_dup_DH
#define DSA_free                                  ADP_DSA_free
#define DSA_get_default_method                    ADP_DSA_get_default_method
#define DSA_get_ex_data                           ADP_DSA_get_ex_data
#define DSA_get_ex_new_index                      ADP_DSA_get_ex_new_index
#define DSA_new                                   ADP_DSA_new
#define DSA_new_method                            ADP_DSA_new_method
#define DSA_set_default_method                    ADP_DSA_set_default_method
#define DSA_set_ex_data                           ADP_DSA_set_ex_data
#define DSA_set_method                            ADP_DSA_set_method
#define DSA_size                                  ADP_DSA_size
#define DSA_up_ref                                ADP_DSA_up_ref
#define DSA_version                               ADP_DSA_version
#define DSA_OpenSSL                               ADP_DSA_OpenSSL
#define DSA_do_sign                               ADP_DSA_do_sign
#define DSA_sign                                  ADP_DSA_sign
#define DSA_sign_setup                            ADP_DSA_sign_setup
#define DSA_do_verify                             ADP_DSA_do_verify
#define DSA_verify                                ADP_DSA_verify
#define DSA_generate_parameters                   ADP_DSA_generate_parameters
#define DSO_METHOD_dl                             ADP_DSO_METHOD_dl
#define DSO_METHOD_dlfcn                          ADP_DSO_METHOD_dlfcn
#define ERR_load_DSO_strings                      ADP_ERR_load_DSO_strings
#define DSO_bind_func                             ADP_DSO_bind_func
#define DSO_bind_var                              ADP_DSO_bind_var
#define DSO_convert_filename                      ADP_DSO_convert_filename
#define DSO_ctrl                                  ADP_DSO_ctrl
#define DSO_flags                                 ADP_DSO_flags
#define DSO_free                                  ADP_DSO_free
#define DSO_get_default_method                    ADP_DSO_get_default_method
#define DSO_get_filename                          ADP_DSO_get_filename
#define DSO_get_loaded_filename                   ADP_DSO_get_loaded_filename
#define DSO_get_method                            ADP_DSO_get_method
#define DSO_load                                  ADP_DSO_load
#define DSO_merge                                 ADP_DSO_merge
#define DSO_new                                   ADP_DSO_new
#define DSO_new_method                            ADP_DSO_new_method
#define DSO_set_default_method                    ADP_DSO_set_default_method
#define DSO_set_filename                          ADP_DSO_set_filename
#define DSO_set_method                            ADP_DSO_set_method
#define DSO_set_name_converter                    ADP_DSO_set_name_converter
#define DSO_up_ref                                ADP_DSO_up_ref
#define DSO_METHOD_null                           ADP_DSO_METHOD_null
#define DSO_METHOD_openssl                        ADP_DSO_METHOD_openssl
#define ERR_add_error_data                        ADP_ERR_add_error_data
#define ERR_clear_error                           ADP_ERR_clear_error
#define ERR_error_string                          ADP_ERR_error_string
#define ERR_error_string_n                        ADP_ERR_error_string_n
#define ERR_free_strings                          ADP_ERR_free_strings
#define ERR_func_error_string                     ADP_ERR_func_error_string
#define ERR_get_err_state_table                   ADP_ERR_get_err_state_table
#define ERR_get_error                             ADP_ERR_get_error
#define ERR_get_error_line                        ADP_ERR_get_error_line
#define ERR_get_error_line_data                   ADP_ERR_get_error_line_data
#define ERR_get_implementation                    ADP_ERR_get_implementation
#define ERR_get_next_error_library                ADP_ERR_get_next_error_library
#define ERR_get_state                             ADP_ERR_get_state
#define ERR_get_string_table                      ADP_ERR_get_string_table
#define ERR_lib_error_string                      ADP_ERR_lib_error_string
#define ERR_load_ERR_strings                      ADP_ERR_load_ERR_strings
#define ERR_load_strings                          ADP_ERR_load_strings
#define ERR_peek_error                            ADP_ERR_peek_error
#define ERR_peek_error_line                       ADP_ERR_peek_error_line
#define ERR_peek_error_line_data                  ADP_ERR_peek_error_line_data
#define ERR_peek_last_error                       ADP_ERR_peek_last_error
#define ERR_peek_last_error_line                  ADP_ERR_peek_last_error_line
#define ERR_peek_last_error_line_data             ADP_ERR_peek_last_error_line_data
#define ERR_pop_to_mark                           ADP_ERR_pop_to_mark
#define ERR_put_error                             ADP_ERR_put_error
#define ERR_reason_error_string                   ADP_ERR_reason_error_string
#define ERR_release_err_state_table               ADP_ERR_release_err_state_table
#define ERR_remove_state                          ADP_ERR_remove_state
#define ERR_set_error_data                        ADP_ERR_set_error_data
#define ERR_set_implementation                    ADP_ERR_set_implementation
#define ERR_set_mark                              ADP_ERR_set_mark
#define ERR_unload_strings                        ADP_ERR_unload_strings
#define ERR_print_errors                          ADP_ERR_print_errors
#define ERR_print_errors_cb                       ADP_ERR_print_errors_cb
#define ERR_print_errors_fp                       ADP_ERR_print_errors_fp
#define ERR_load_crypto_strings                   ADP_ERR_load_crypto_strings
#define pCryptoErrInitRtn                         ADP_pCryptoErrInitRtn
#define BIO_f_base64                              ADP_BIO_f_base64
#define BIO_f_cipher                              ADP_BIO_f_cipher
#define BIO_set_cipher                            ADP_BIO_set_cipher
#define BIO_f_md                                  ADP_BIO_f_md
#define BIO_f_reliable                            ADP_BIO_f_reliable
#define EVP_Digest                                ADP_EVP_Digest
#define EVP_DigestFinal                           ADP_EVP_DigestFinal
#define EVP_DigestFinal_ex                        ADP_EVP_DigestFinal_ex
#define EVP_DigestInit                            ADP_EVP_DigestInit
#define EVP_DigestInit_ex                         ADP_EVP_DigestInit_ex
#define EVP_DigestUpdate                          ADP_EVP_DigestUpdate
#define EVP_MD_CTX_cleanup                        ADP_EVP_MD_CTX_cleanup
#define EVP_MD_CTX_copy                           ADP_EVP_MD_CTX_copy
#define EVP_MD_CTX_copy_ex                        ADP_EVP_MD_CTX_copy_ex
#define EVP_MD_CTX_create                         ADP_EVP_MD_CTX_create
#define EVP_MD_CTX_destroy                        ADP_EVP_MD_CTX_destroy
#define EVP_MD_CTX_init                           ADP_EVP_MD_CTX_init
#define EVP_aes_128_cbc                           ADP_EVP_aes_128_cbc
#define EVP_aes_128_ccm                           ADP_EVP_aes_128_ccm
#define EVP_aes_128_cfb1                          ADP_EVP_aes_128_cfb1
#define EVP_aes_128_cfb128                        ADP_EVP_aes_128_cfb128
#define EVP_aes_128_cfb8                          ADP_EVP_aes_128_cfb8
#define EVP_aes_128_ecb                           ADP_EVP_aes_128_ecb
#define EVP_aes_128_ofb                           ADP_EVP_aes_128_ofb
#define EVP_aes_192_cbc                           ADP_EVP_aes_192_cbc
#define EVP_aes_192_ccm                           ADP_EVP_aes_192_ccm
#define EVP_aes_192_cfb1                          ADP_EVP_aes_192_cfb1
#define EVP_aes_192_cfb128                        ADP_EVP_aes_192_cfb128
#define EVP_aes_192_cfb8                          ADP_EVP_aes_192_cfb8
#define EVP_aes_192_ecb                           ADP_EVP_aes_192_ecb
#define EVP_aes_192_ofb                           ADP_EVP_aes_192_ofb
#define EVP_aes_256_cbc                           ADP_EVP_aes_256_cbc
#define EVP_aes_256_ccm                           ADP_EVP_aes_256_ccm
#define EVP_aes_256_cfb1                          ADP_EVP_aes_256_cfb1
#define EVP_aes_256_cfb128                        ADP_EVP_aes_256_cfb128
#define EVP_aes_256_cfb8                          ADP_EVP_aes_256_cfb8
#define EVP_aes_256_ecb                           ADP_EVP_aes_256_ecb
#define EVP_aes_256_ofb                           ADP_EVP_aes_256_ofb
#define EVP_bf_cbc                                ADP_EVP_bf_cbc
#define EVP_bf_cfb64                              ADP_EVP_bf_cfb64
#define EVP_bf_ecb                                ADP_EVP_bf_ecb
#define EVP_bf_ofb                                ADP_EVP_bf_ofb
#define EVP_cast5_cbc                             ADP_EVP_cast5_cbc
#define EVP_cast5_cfb64                           ADP_EVP_cast5_cfb64
#define EVP_cast5_ecb                             ADP_EVP_cast5_ecb
#define EVP_cast5_ofb                             ADP_EVP_cast5_ofb
#define EVP_des_cbc                               ADP_EVP_des_cbc
#define EVP_des_cfb1                              ADP_EVP_des_cfb1
#define EVP_des_cfb64                             ADP_EVP_des_cfb64
#define EVP_des_cfb8                              ADP_EVP_des_cfb8
#define EVP_des_ecb                               ADP_EVP_des_ecb
#define EVP_des_ofb                               ADP_EVP_des_ofb
#define EVP_des_ede                               ADP_EVP_des_ede
#define EVP_des_ede3                              ADP_EVP_des_ede3
#define EVP_des_ede3_cbc                          ADP_EVP_des_ede3_cbc
#define EVP_des_ede3_cfb1                         ADP_EVP_des_ede3_cfb1
#define EVP_des_ede3_cfb64                        ADP_EVP_des_ede3_cfb64
#define EVP_des_ede3_cfb8                         ADP_EVP_des_ede3_cfb8
#define EVP_des_ede3_ecb                          ADP_EVP_des_ede3_ecb
#define EVP_des_ede3_ofb                          ADP_EVP_des_ede3_ofb
#define EVP_des_ede_cbc                           ADP_EVP_des_ede_cbc
#define EVP_des_ede_cfb64                         ADP_EVP_des_ede_cfb64
#define EVP_des_ede_ecb                           ADP_EVP_des_ede_ecb
#define EVP_des_ede_ofb                           ADP_EVP_des_ede_ofb
#define EVP_rc2_40_cbc                            ADP_EVP_rc2_40_cbc
#define EVP_rc2_64_cbc                            ADP_EVP_rc2_64_cbc
#define EVP_rc2_cbc                               ADP_EVP_rc2_cbc
#define EVP_rc2_cfb64                             ADP_EVP_rc2_cfb64
#define EVP_rc2_ecb                               ADP_EVP_rc2_ecb
#define EVP_rc2_ofb                               ADP_EVP_rc2_ofb
#define EVP_enc_null                              ADP_EVP_enc_null
#define EVP_rc4                                   ADP_EVP_rc4
#define EVP_rc4_40                                ADP_EVP_rc4_40
#define EVP_desx_cbc                              ADP_EVP_desx_cbc
#define EVP_DecodeBlock                           ADP_EVP_DecodeBlock
#define EVP_DecodeFinal                           ADP_EVP_DecodeFinal
#define EVP_DecodeInit                            ADP_EVP_DecodeInit
#define EVP_DecodeUpdate                          ADP_EVP_DecodeUpdate
#define EVP_EncodeBlock                           ADP_EVP_EncodeBlock
#define EVP_EncodeFinal                           ADP_EVP_EncodeFinal
#define EVP_EncodeInit                            ADP_EVP_EncodeInit
#define EVP_EncodeUpdate                          ADP_EVP_EncodeUpdate
#define EVP_CIPHER_CTX_cleanup                    ADP_EVP_CIPHER_CTX_cleanup
#define EVP_CIPHER_CTX_ctrl                       ADP_EVP_CIPHER_CTX_ctrl
#define EVP_CIPHER_CTX_free                       ADP_EVP_CIPHER_CTX_free
#define EVP_CIPHER_CTX_init                       ADP_EVP_CIPHER_CTX_init
#define EVP_CIPHER_CTX_new                        ADP_EVP_CIPHER_CTX_new
#define EVP_CIPHER_CTX_rand_key                   ADP_EVP_CIPHER_CTX_rand_key
#define EVP_CIPHER_CTX_set_key_length             ADP_EVP_CIPHER_CTX_set_key_length
#define EVP_CIPHER_CTX_set_padding                ADP_EVP_CIPHER_CTX_set_padding
#define EVP_CipherFinal                           ADP_EVP_CipherFinal
#define EVP_CipherFinal_ex                        ADP_EVP_CipherFinal_ex
#define EVP_CipherInit                            ADP_EVP_CipherInit
#define EVP_CipherInit_ex                         ADP_EVP_CipherInit_ex
#define EVP_CipherUpdate                          ADP_EVP_CipherUpdate
#define EVP_DecryptFinal                          ADP_EVP_DecryptFinal
#define EVP_DecryptFinal_ex                       ADP_EVP_DecryptFinal_ex
#define EVP_DecryptInit                           ADP_EVP_DecryptInit
#define EVP_DecryptInit_ex                        ADP_EVP_DecryptInit_ex
#define EVP_DecryptUpdate                         ADP_EVP_DecryptUpdate
#define EVP_EncryptFinal                          ADP_EVP_EncryptFinal
#define EVP_EncryptFinal_ex                       ADP_EVP_EncryptFinal_ex
#define EVP_EncryptInit                           ADP_EVP_EncryptInit
#define EVP_EncryptInit_ex                        ADP_EVP_EncryptInit_ex
#define EVP_EncryptUpdate                         ADP_EVP_EncryptUpdate
#define EVP_version                               ADP_EVP_version
#define ERR_load_EVP_strings                      ADP_ERR_load_EVP_strings
#define EVP_BytesToKey                            ADP_EVP_BytesToKey
#define EVP_get_pw_prompt                         ADP_EVP_get_pw_prompt
#define EVP_read_pw_string                        ADP_EVP_read_pw_string
#define EVP_set_pw_prompt                         ADP_EVP_set_pw_prompt
#define EVP_CIPHER_CTX_block_size                 ADP_EVP_CIPHER_CTX_block_size
#define EVP_CIPHER_CTX_cipher                     ADP_EVP_CIPHER_CTX_cipher
#define EVP_CIPHER_CTX_flags                      ADP_EVP_CIPHER_CTX_flags
#define EVP_CIPHER_CTX_get_app_data               ADP_EVP_CIPHER_CTX_get_app_data
#define EVP_CIPHER_CTX_iv_length                  ADP_EVP_CIPHER_CTX_iv_length
#define EVP_CIPHER_CTX_key_length                 ADP_EVP_CIPHER_CTX_key_length
#define EVP_CIPHER_CTX_nid                        ADP_EVP_CIPHER_CTX_nid
#define EVP_CIPHER_CTX_set_app_data               ADP_EVP_CIPHER_CTX_set_app_data
#define EVP_CIPHER_asn1_to_param                  ADP_EVP_CIPHER_asn1_to_param
#define EVP_CIPHER_block_size                     ADP_EVP_CIPHER_block_size
#define EVP_CIPHER_flags                          ADP_EVP_CIPHER_flags
#define EVP_CIPHER_get_asn1_iv                    ADP_EVP_CIPHER_get_asn1_iv
#define EVP_CIPHER_iv_length                      ADP_EVP_CIPHER_iv_length
#define EVP_CIPHER_key_length                     ADP_EVP_CIPHER_key_length
#define EVP_CIPHER_nid                            ADP_EVP_CIPHER_nid
#define EVP_CIPHER_param_to_asn1                  ADP_EVP_CIPHER_param_to_asn1
#define EVP_CIPHER_set_asn1_iv                    ADP_EVP_CIPHER_set_asn1_iv
#define EVP_CIPHER_type                           ADP_EVP_CIPHER_type
#define EVP_Cipher                                ADP_EVP_Cipher
#define EVP_MD_CTX_clear_flags                    ADP_EVP_MD_CTX_clear_flags
#define EVP_MD_CTX_md                             ADP_EVP_MD_CTX_md
#define EVP_MD_CTX_set_flags                      ADP_EVP_MD_CTX_set_flags
#define EVP_MD_CTX_test_flags                     ADP_EVP_MD_CTX_test_flags
#define EVP_MD_block_size                         ADP_EVP_MD_block_size
#define EVP_MD_pkey_type                          ADP_EVP_MD_pkey_type
#define EVP_MD_size                               ADP_EVP_MD_size
#define EVP_MD_type                               ADP_EVP_MD_type
#define EVP_PBE_CipherInit                        ADP_EVP_PBE_CipherInit
#define EVP_PBE_alg_add                           ADP_EVP_PBE_alg_add
#define EVP_PBE_cleanup                           ADP_EVP_PBE_cleanup
#define EVP_md4                                   ADP_EVP_md4
#define EVP_dss                                   ADP_EVP_dss
#define EVP_dss1                                  ADP_EVP_dss1
#define EVP_md5                                   ADP_EVP_md5
#define EVP_md_null                               ADP_EVP_md_null
#define EVP_ripemd160                             ADP_EVP_ripemd160
#define EVP_sha                                   ADP_EVP_sha
#define EVP_sha1                                  ADP_EVP_sha1
#define EVP_sha224                                ADP_EVP_sha224
#define EVP_sha256                                ADP_EVP_sha256
#define EVP_sha384                                ADP_EVP_sha384
#define EVP_sha512                                ADP_EVP_sha512
#define EVP_add_cipher                            ADP_EVP_add_cipher
#define EVP_add_digest                            ADP_EVP_add_digest
#define EVP_cleanup                               ADP_EVP_cleanup
#define EVP_get_cipherbyname                      ADP_EVP_get_cipherbyname
#define EVP_get_digestbyname                      ADP_EVP_get_digestbyname
#define EVP_PKEY_decrypt                          ADP_EVP_PKEY_decrypt
#define EVP_PKEY_encrypt                          ADP_EVP_PKEY_encrypt
#define EVP_PKEY_assign                           ADP_EVP_PKEY_assign
#define EVP_PKEY_bits                             ADP_EVP_PKEY_bits
#define EVP_PKEY_cmp                              ADP_EVP_PKEY_cmp
#define EVP_PKEY_cmp_parameters                   ADP_EVP_PKEY_cmp_parameters
#define EVP_PKEY_copy_parameters                  ADP_EVP_PKEY_copy_parameters
#define EVP_PKEY_free                             ADP_EVP_PKEY_free
#define EVP_PKEY_get1_DH                          ADP_EVP_PKEY_get1_DH
#define EVP_PKEY_get1_DSA                         ADP_EVP_PKEY_get1_DSA
#define EVP_PKEY_get1_RSA                         ADP_EVP_PKEY_get1_RSA
#define EVP_PKEY_missing_parameters               ADP_EVP_PKEY_missing_parameters
#define EVP_PKEY_new                              ADP_EVP_PKEY_new
#define EVP_PKEY_save_parameters                  ADP_EVP_PKEY_save_parameters
#define EVP_PKEY_set1_DH                          ADP_EVP_PKEY_set1_DH
#define EVP_PKEY_set1_DSA                         ADP_EVP_PKEY_set1_DSA
#define EVP_PKEY_set1_RSA                         ADP_EVP_PKEY_set1_RSA
#define EVP_PKEY_size                             ADP_EVP_PKEY_size
#define EVP_PKEY_type                             ADP_EVP_PKEY_type
#define EVP_OpenFinal                             ADP_EVP_OpenFinal
#define EVP_OpenInit                              ADP_EVP_OpenInit
#define EVP_SealFinal                             ADP_EVP_SealFinal
#define EVP_SealInit                              ADP_EVP_SealInit
#define EVP_SignFinal                             ADP_EVP_SignFinal
#define EVP_VerifyFinal                           ADP_EVP_VerifyFinal
#define OPENSSL_add_all_algorithms_conf           ADP_OPENSSL_add_all_algorithms_conf
#define EVP_ecdsa                                 ADP_EVP_ecdsa
#define EVP_aes_128_cfb128                        ADP_EVP_aes_128_cfb128
#define EVP_aes_192_cfb128                        ADP_EVP_aes_192_cfb128
#define EVP_aes_256_cfb128                        ADP_EVP_aes_256_cfb128
#define EVP_bf_cfb64                              ADP_EVP_bf_cfb64
#define EVP_cast5_cfb64                           ADP_EVP_cast5_cfb64
#define EVP_des_cfb64                             ADP_EVP_des_cfb64
#define EVP_des_ede3_cfb64                        ADP_EVP_des_ede3_cfb64
#define EVP_des_ede_cfb64                         ADP_EVP_des_ede_cfb64
#define EVP_rc2_cfb64                             ADP_EVP_rc2_cfb64
#define OPENSSL_add_all_algorithms_noconf         ADP_OPENSSL_add_all_algorithms_noconf
#define OpenSSL_add_all_ciphers                   ADP_OpenSSL_add_all_ciphers
#define OpenSSL_add_all_digests                   ADP_OpenSSL_add_all_digests
#define HMAC                                      ADP_HMAC
#define HMAC_CTX_cleanup                          ADP_HMAC_CTX_cleanup
#define HMAC_CTX_init                             ADP_HMAC_CTX_init
#define HMAC_Final                                ADP_HMAC_Final
#define HMAC_Init                                 ADP_HMAC_Init
#define HMAC_Init_ex                              ADP_HMAC_Init_ex
#define HMAC_Update                               ADP_HMAC_Update
#define lh_delete                                 ADP_lh_delete
#define lh_doall                                  ADP_lh_doall
#define lh_doall_arg                              ADP_lh_doall_arg
#define lh_free                                   ADP_lh_free
#define lh_insert                                 ADP_lh_insert
#define lh_new                                    ADP_lh_new
#define lh_num_items                              ADP_lh_num_items
#define lh_retrieve                               ADP_lh_retrieve
#define lh_strhash                                ADP_lh_strhash
#define lh_version                                ADP_lh_version
#define lh_node_stats                             ADP_lh_node_stats
#define lh_node_stats_bio                         ADP_lh_node_stats_bio
#define lh_node_usage_stats                       ADP_lh_node_usage_stats
#define lh_node_usage_stats_bio                   ADP_lh_node_usage_stats_bio
#define lh_stats                                  ADP_lh_stats
#define lh_stats_bio                              ADP_lh_stats_bio
#define MD4_Final                                 ADP_MD4_Final
#define MD4_Init                                  ADP_MD4_Init
#define MD4_Transform                             ADP_MD4_Transform
#define MD4_Update                                ADP_MD4_Update
#define MD4_version                               ADP_MD4_version
#define md4_block_data_order                      ADP_md4_block_data_order
#define md4_block_host_order                      ADP_md4_block_host_order
#define MD4                                       ADP_MD4
#define MD5                                       ADP_MD5
#define MD5_Final                                 ADP_MD5_Final
#define MD5_Init                                  ADP_MD5_Init
#define MD5_Transform                             ADP_MD5_Transform
#define MD5_Update                                ADP_MD5_Update
#define MD5_version                               ADP_MD5_version
#define md5_block_data_order                      ADP_md5_block_data_order
#define md5_block_host_order                      ADP_md5_block_host_order
#define OBJ_NAME_add                              ADP_OBJ_NAME_add
#define OBJ_NAME_cleanup                          ADP_OBJ_NAME_cleanup
#define OBJ_NAME_do_all                           ADP_OBJ_NAME_do_all
#define OBJ_NAME_do_all_sorted                    ADP_OBJ_NAME_do_all_sorted
#define OBJ_NAME_get                              ADP_OBJ_NAME_get
#define OBJ_NAME_init                             ADP_OBJ_NAME_init
#define OBJ_NAME_new_index                        ADP_OBJ_NAME_new_index
#define OBJ_NAME_remove                           ADP_OBJ_NAME_remove
#define OBJ_add_object                            ADP_OBJ_add_object
#define OBJ_bsearch                               ADP_OBJ_bsearch
#define OBJ_bsearch_ex                            ADP_OBJ_bsearch_ex
#define OBJ_cleanup                               ADP_OBJ_cleanup
#define OBJ_create                                ADP_OBJ_create
#define OBJ_create_objects                        ADP_OBJ_create_objects
#define OBJ_ln2nid                                ADP_OBJ_ln2nid
#define OBJ_new_nid                               ADP_OBJ_new_nid
#define OBJ_nid2ln                                ADP_OBJ_nid2ln
#define OBJ_nid2obj                               ADP_OBJ_nid2obj
#define OBJ_nid2sn                                ADP_OBJ_nid2sn
#define OBJ_obj2nid                               ADP_OBJ_obj2nid
#define OBJ_obj2txt                               ADP_OBJ_obj2txt
#define OBJ_sn2nid                                ADP_OBJ_sn2nid
#define OBJ_txt2nid                               ADP_OBJ_txt2nid
#define OBJ_txt2obj                               ADP_OBJ_txt2obj
#define OBJ_cmp                                   ADP_OBJ_cmp
#define OBJ_dup                                   ADP_OBJ_dup
#define ERR_load_OBJ_strings                      ADP_ERR_load_OBJ_strings
#define ERR_load_PEM_strings                      ADP_ERR_load_PEM_strings
#define PEM_ASN1_read                             ADP_PEM_ASN1_read
#define PEM_ASN1_write                            ADP_PEM_ASN1_write
#define PEM_ASN1_write_bio                        ADP_PEM_ASN1_write_bio
#define PEM_bytes_read_bio                        ADP_PEM_bytes_read_bio
#define PEM_def_callback                          ADP_PEM_def_callback
#define PEM_dek_info                              ADP_PEM_dek_info
#define PEM_do_header                             ADP_PEM_do_header
#define PEM_get_EVP_CIPHER_INFO                   ADP_PEM_get_EVP_CIPHER_INFO
#define PEM_proc_type                             ADP_PEM_proc_type
#define PEM_read                                  ADP_PEM_read
#define PEM_read_bio                              ADP_PEM_read_bio
#define PEM_version                               ADP_PEM_version
#define PEM_write                                 ADP_PEM_write
#define PEM_write_bio                             ADP_PEM_write_bio
#define PEM_ASN1_read_bio                         ADP_PEM_ASN1_read_bio
#define PEM_SealFinal                             ADP_PEM_SealFinal
#define PEM_SealInit                              ADP_PEM_SealInit
#define PEM_SealUpdate                            ADP_PEM_SealUpdate
#define PEM_read_DHparams                         ADP_PEM_read_DHparams
#define PEM_read_DSAPrivateKey                    ADP_PEM_read_DSAPrivateKey
#define PEM_read_DSAparams                        ADP_PEM_read_DSAparams
#define PEM_read_RSAPrivateKey                    ADP_PEM_read_RSAPrivateKey
#define PEM_read_RSAPublicKey                     ADP_PEM_read_RSAPublicKey
#define PEM_read_bio_DHparams                     ADP_PEM_read_bio_DHparams
#define PEM_read_bio_DSAPrivateKey                ADP_PEM_read_bio_DSAPrivateKey
#define PEM_read_bio_DSAparams                    ADP_PEM_read_bio_DSAparams
#define PEM_read_bio_RSAPrivateKey                ADP_PEM_read_bio_RSAPrivateKey
#define PEM_read_bio_RSAPublicKey                 ADP_PEM_read_bio_RSAPublicKey
#define PEM_write_DHparams                        ADP_PEM_write_DHparams
#define PEM_write_DSAPrivateKey                   ADP_PEM_write_DSAPrivateKey
#define PEM_write_DSAparams                       ADP_PEM_write_DSAparams
#define PEM_write_PrivateKey                      ADP_PEM_write_PrivateKey
#define PEM_write_RSAPrivateKey                   ADP_PEM_write_RSAPrivateKey
#define PEM_write_RSAPublicKey                    ADP_PEM_write_RSAPublicKey
#define PEM_write_bio_DHparams                    ADP_PEM_write_bio_DHparams
#define PEM_write_bio_DSAPrivateKey               ADP_PEM_write_bio_DSAPrivateKey
#define PEM_write_bio_DSAparams                   ADP_PEM_write_bio_DSAparams
#define PEM_write_bio_PrivateKey                  ADP_PEM_write_bio_PrivateKey
#define PEM_write_bio_RSAPrivateKey               ADP_PEM_write_bio_RSAPrivateKey
#define PEM_write_bio_RSAPublicKey                ADP_PEM_write_bio_RSAPublicKey
#define PEM_read_PrivateKey                       ADP_PEM_read_PrivateKey
#define PEM_read_bio_PrivateKey                   ADP_PEM_read_bio_PrivateKey
#define pitem_free                                ADP_pitem_free
#define pitem_new                                 ADP_pitem_new
#define pqueue_find                               ADP_pqueue_find
#define pqueue_free                               ADP_pqueue_free
#define pqueue_insert                             ADP_pqueue_insert
#define pqueue_iterator                           ADP_pqueue_iterator
#define pqueue_new                                ADP_pqueue_new
#define pqueue_next                               ADP_pqueue_next
#define pqueue_peek                               ADP_pqueue_peek
#define pqueue_pop                                ADP_pqueue_pop
#define RAND_SSLeay                               ADP_RAND_SSLeay
#define RAND_version                              ADP_RAND_version
#define rand_ssleay_meth                          ADP_rand_ssleay_meth
#define RAND_egd                                  ADP_RAND_egd
#define RAND_egd_bytes                            ADP_RAND_egd_bytes
#define RAND_query_egd_bytes                      ADP_RAND_query_egd_bytes
#define ERR_load_RAND_strings                     ADP_ERR_load_RAND_strings
#define RAND_add                                  ADP_RAND_add
#define RAND_bytes                                ADP_RAND_bytes
#define RAND_cleanup                              ADP_RAND_cleanup
#define RAND_get_rand_method                      ADP_RAND_get_rand_method
#define RAND_pseudo_bytes                         ADP_RAND_pseudo_bytes
#define RAND_seed                                 ADP_RAND_seed
#define RAND_set_rand_method                      ADP_RAND_set_rand_method
#define RAND_status                               ADP_RAND_status
#define adpKlitePRNGInit                          ADP_adpKlitePRNGInit
#define klitePRNGExit                             ADP_klitePRNGExit
#define klitePRNGGen                              ADP_klitePRNGGen
#define klitePRNGSeed                             ADP_klitePRNGSeed
#define RAND_file_name                            ADP_RAND_file_name
#define RAND_load_file                            ADP_RAND_load_file
#define RAND_write_file                           ADP_RAND_write_file
#define RAND_poll                                 ADP_RAND_poll
#define RC2_cbc_encrypt                           ADP_RC2_cbc_encrypt
#define RC2_decrypt                               ADP_RC2_decrypt
#define RC2_encrypt                               ADP_RC2_encrypt
#define RC2_ecb_encrypt                           ADP_RC2_ecb_encrypt
#define RC2_version                               ADP_RC2_version
#define RC2_set_key                               ADP_RC2_set_key
#define RC2_cfb64_encrypt                         ADP_RC2_cfb64_encrypt
#define RC2_ofb64_encrypt                         ADP_RC2_ofb64_encrypt
#define RC4                                       ADP_RC4
#define RC4_options                               ADP_RC4_options
#define RC4_set_key                               ADP_RC4_set_key
#define RC4_version                               ADP_RC4_version
#define RIPEMD160_Final                           ADP_RIPEMD160_Final
#define RIPEMD160_Init                            ADP_RIPEMD160_Init
#define RIPEMD160_Transform                       ADP_RIPEMD160_Transform
#define RIPEMD160_Update                          ADP_RIPEMD160_Update
#define RMD160_version                            ADP_RMD160_version
#define RIPEMD160                                 ADP_RIPEMD160
#define RSAPrivateKey_asn1_meth                   ADP_RSAPrivateKey_asn1_meth
#define RSAPrivateKey_dup                         ADP_RSAPrivateKey_dup
#define RSAPrivateKey_it                          ADP_RSAPrivateKey_it
#define RSAPublicKey_dup                          ADP_RSAPublicKey_dup
#define RSAPublicKey_it                           ADP_RSAPublicKey_it
#define d2i_RSAPrivateKey                         ADP_d2i_RSAPrivateKey
#define d2i_RSAPublicKey                          ADP_d2i_RSAPublicKey
#define i2d_RSAPrivateKey                         ADP_i2d_RSAPrivateKey
#define i2d_RSAPublicKey                          ADP_i2d_RSAPublicKey
#define RSA_check_key                             ADP_RSA_check_key
#define RSA_PKCS1_SSLeay                          ADP_RSA_PKCS1_SSLeay
#define ERR_load_RSA_strings                      ADP_ERR_load_RSA_strings
#define RSA_generate_key_ex                       ADP_RSA_generate_key_ex
#define RSA_blinding_off                          ADP_RSA_blinding_off
#define RSA_blinding_on                           ADP_RSA_blinding_on
#define RSA_flags                                 ADP_RSA_flags
#define RSA_free                                  ADP_RSA_free
#define RSA_get_default_method                    ADP_RSA_get_default_method
#define RSA_get_ex_data                           ADP_RSA_get_ex_data
#define RSA_get_ex_new_index                      ADP_RSA_get_ex_new_index
#define RSA_get_method                            ADP_RSA_get_method
#define RSA_memory_lock                           ADP_RSA_memory_lock
#define RSA_new                                   ADP_RSA_new
#define RSA_new_method                            ADP_RSA_new_method
#define RSA_private_decrypt                       ADP_RSA_private_decrypt
#define RSA_private_encrypt                       ADP_RSA_private_encrypt
#define RSA_public_decrypt                        ADP_RSA_public_decrypt
#define RSA_public_encrypt                        ADP_RSA_public_encrypt
#define RSA_set_default_method                    ADP_RSA_set_default_method
#define RSA_set_ex_data                           ADP_RSA_set_ex_data
#define RSA_set_method                            ADP_RSA_set_method
#define RSA_setup_blinding                        ADP_RSA_setup_blinding
#define RSA_size                                  ADP_RSA_size
#define RSA_up_ref                                ADP_RSA_up_ref
#define RSA_version                               ADP_RSA_version
#define RSA_padding_add_none                      ADP_RSA_padding_add_none
#define RSA_padding_check_none                    ADP_RSA_padding_check_none
#define RSA_null_method                           ADP_RSA_null_method
#define MGF1                                      ADP_MGF1
#define PKCS1_MGF1                                ADP_PKCS1_MGF1
#define RSA_padding_add_PKCS1_OAEP                ADP_RSA_padding_add_PKCS1_OAEP
#define RSA_padding_check_PKCS1_OAEP              ADP_RSA_padding_check_PKCS1_OAEP
#define RSA_padding_add_PKCS1_type_1              ADP_RSA_padding_add_PKCS1_type_1
#define RSA_padding_add_PKCS1_type_2              ADP_RSA_padding_add_PKCS1_type_2
#define RSA_padding_check_PKCS1_type_1            ADP_RSA_padding_check_PKCS1_type_1
#define RSA_padding_check_PKCS1_type_2            ADP_RSA_padding_check_PKCS1_type_2
#define RSA_sign_ASN1_OCTET_STRING                ADP_RSA_sign_ASN1_OCTET_STRING
#define RSA_verify_ASN1_OCTET_STRING              ADP_RSA_verify_ASN1_OCTET_STRING
#define RSA_sign                                  ADP_RSA_sign
#define RSA_verify                                ADP_RSA_verify
#define RSA_padding_add_SSLv23                    ADP_RSA_padding_add_SSLv23
#define RSA_padding_check_SSLv23                  ADP_RSA_padding_check_SSLv23
#define RSA_X931_hash_id                          ADP_RSA_X931_hash_id
#define RSA_padding_add_X931                      ADP_RSA_padding_add_X931
#define RSA_padding_check_X931                    ADP_RSA_padding_check_X931
#define RSA_generate_key                          ADP_RSA_generate_key
#define SHA1_Final                                ADP_SHA1_Final
#define SHA1_Init                                 ADP_SHA1_Init
#define SHA1_Transform                            ADP_SHA1_Transform
#define SHA1_Update                               ADP_SHA1_Update
#define SHA1_version                              ADP_SHA1_version
#define sha1_block_data_order                     ADP_sha1_block_data_order
#define sha1_block_host_order                     ADP_sha1_block_host_order
#define SHA1                                      ADP_SHA1
#define SHA_Final                                 ADP_SHA_Final
#define SHA_Init                                  ADP_SHA_Init
#define SHA_Transform                             ADP_SHA_Transform
#define SHA_Update                                ADP_SHA_Update
#define SHA_version                               ADP_SHA_version
#define sha_block_data_order                      ADP_sha_block_data_order
#define sha_block_host_order                      ADP_sha_block_host_order
#define SHA                                       ADP_SHA
#define SHA224                                    ADP_SHA224
#define SHA224_Final                              ADP_SHA224_Final
#define SHA224_Init                               ADP_SHA224_Init
#define SHA224_Update                             ADP_SHA224_Update
#define SHA256                                    ADP_SHA256
#define SHA256_Final                              ADP_SHA256_Final
#define SHA256_Init                               ADP_SHA256_Init
#define SHA256_Transform                          ADP_SHA256_Transform
#define SHA256_Update                             ADP_SHA256_Update
#define SHA256_version                            ADP_SHA256_version
#define sha256_block_data_order                   ADP_sha256_block_data_order
#define sha256_block_host_order                   ADP_sha256_block_host_order
#define SHA384                                    ADP_SHA384
#define SHA384_Final                              ADP_SHA384_Final
#define SHA384_Init                               ADP_SHA384_Init
#define SHA384_Update                             ADP_SHA384_Update
#define SHA512                                    ADP_SHA512
#define SHA512_Final                              ADP_SHA512_Final
#define SHA512_Init                               ADP_SHA512_Init
#define SHA512_Transform                          ADP_SHA512_Transform
#define SHA512_Update                             ADP_SHA512_Update
#define SHA512_version                            ADP_SHA512_version
#define STACK_version                             ADP_STACK_version
#define sk_delete                                 ADP_sk_delete
#define sk_delete_ptr                             ADP_sk_delete_ptr
#define sk_dup                                    ADP_sk_dup
#define sk_find                                   ADP_sk_find
#define sk_find_ex                                ADP_sk_find_ex
#define sk_free                                   ADP_sk_free
#define sk_insert                                 ADP_sk_insert
#define sk_is_sorted                              ADP_sk_is_sorted
#define sk_new                                    ADP_sk_new
#define sk_new_null                               ADP_sk_new_null
#define sk_num                                    ADP_sk_num
#define sk_pop                                    ADP_sk_pop
#define sk_pop_free                               ADP_sk_pop_free
#define sk_push                                   ADP_sk_push
#define sk_set                                    ADP_sk_set
#define sk_set_cmp_func                           ADP_sk_set_cmp_func
#define sk_shift                                  ADP_sk_shift
#define sk_sort                                   ADP_sk_sort
#define sk_unshift                                ADP_sk_unshift
#define sk_value                                  ADP_sk_value
#define sk_zero                                   ADP_sk_zero
#define TXT_DB_create_index                       ADP_TXT_DB_create_index
#define TXT_DB_free                               ADP_TXT_DB_free
#define TXT_DB_get_by_index                       ADP_TXT_DB_get_by_index
#define TXT_DB_insert                             ADP_TXT_DB_insert
#define TXT_DB_read                               ADP_TXT_DB_read
#define TXT_DB_version                            ADP_TXT_DB_version
#define TXT_DB_write                              ADP_TXT_DB_write
#define _ossl_old_des_read_pw                     ADP__ossl_old_des_read_pw
#define _ossl_old_des_read_pw_string              ADP__ossl_old_des_read_pw_string
#define ERR_load_UI_strings                       ADP_ERR_load_UI_strings
#define UI_add_error_string                       ADP_UI_add_error_string
#define UI_add_info_string                        ADP_UI_add_info_string
#define UI_add_input_boolean                      ADP_UI_add_input_boolean
#define UI_add_input_string                       ADP_UI_add_input_string
#define UI_add_user_data                          ADP_UI_add_user_data
#define UI_add_verify_string                      ADP_UI_add_verify_string
#define UI_construct_prompt                       ADP_UI_construct_prompt
#define UI_create_method                          ADP_UI_create_method
#define UI_ctrl                                   ADP_UI_ctrl
#define UI_destroy_method                         ADP_UI_destroy_method
#define UI_dup_error_string                       ADP_UI_dup_error_string
#define UI_dup_info_string                        ADP_UI_dup_info_string
#define UI_dup_input_boolean                      ADP_UI_dup_input_boolean
#define UI_dup_input_string                       ADP_UI_dup_input_string
#define UI_dup_verify_string                      ADP_UI_dup_verify_string
#define UI_free                                   ADP_UI_free
#define UI_get0_action_string                     ADP_UI_get0_action_string
#define UI_get0_output_string                     ADP_UI_get0_output_string
#define UI_get0_result                            ADP_UI_get0_result
#define UI_get0_result_string                     ADP_UI_get0_result_string
#define UI_get0_test_string                       ADP_UI_get0_test_string
#define UI_get0_user_data                         ADP_UI_get0_user_data
#define UI_get_default_method                     ADP_UI_get_default_method
#define UI_get_ex_data                            ADP_UI_get_ex_data
#define UI_get_ex_new_index                       ADP_UI_get_ex_new_index
#define UI_get_input_flags                        ADP_UI_get_input_flags
#define UI_get_method                             ADP_UI_get_method
#define UI_get_result_maxsize                     ADP_UI_get_result_maxsize
#define UI_get_result_minsize                     ADP_UI_get_result_minsize
#define UI_get_string_type                        ADP_UI_get_string_type
#define UI_method_get_closer                      ADP_UI_method_get_closer
#define UI_method_get_flusher                     ADP_UI_method_get_flusher
#define UI_method_get_opener                      ADP_UI_method_get_opener
#define UI_method_get_reader                      ADP_UI_method_get_reader
#define UI_method_get_writer                      ADP_UI_method_get_writer
#define UI_method_set_closer                      ADP_UI_method_set_closer
#define UI_method_set_flusher                     ADP_UI_method_set_flusher
#define UI_method_set_opener                      ADP_UI_method_set_opener
#define UI_method_set_reader                      ADP_UI_method_set_reader
#define UI_method_set_writer                      ADP_UI_method_set_writer
#define UI_new                                    ADP_UI_new
#define UI_new_method                             ADP_UI_new_method
#define UI_process                                ADP_UI_process
#define UI_set_default_method                     ADP_UI_set_default_method
#define UI_set_ex_data                            ADP_UI_set_ex_data
#define UI_set_method                             ADP_UI_set_method
#define UI_set_result                             ADP_UI_set_result
#define UI_OpenSSL                                ADP_UI_OpenSSL
#define UI_UTIL_read_pw                           ADP_UI_UTIL_read_pw
#define UI_UTIL_read_pw_string                    ADP_UI_UTIL_read_pw_string
#define opensslCryptoApiLibInit                   ADP_opensslCryptoApiLibInit
#define opensslCryptoLockInstall                  ADP_opensslCryptoLockInstall
#define opensslCryptoLockUnInstall                ADP_opensslCryptoLockUnInstall
#define cryptoPartIdInit                          ADP_cryptoPartIdInit
#define cryptoSupportMemInit                      ADP_cryptoSupportMemInit
#define kliteEngineErrorTableInit                 ADP_kliteEngineErrorTableInit
#define kliteErrorTableInit                       ADP_kliteErrorTableInit
#define kliteX509ErrorTableInit                   ADP_kliteX509ErrorTableInit
#define PKCS12_PBE_add                            ADP_PKCS12_PBE_add
#define PKCS5_PBE_add                             ADP_PKCS5_PBE_add
#define SSL_library_init                          ADP_SSL_library_init
#define SSL_load_error_strings                    ADP_SSL_load_error_strings
#define X509_get_default_cert_area                ADP_X509_get_default_cert_area
#define kliteInitStatus                           ADP_kliteInitStatus
#define kliteLibInit                              ADP_kliteLibInit
#define kliteAESInit                              ADP_kliteAESInit
#define kliteBlowFishInit                         ADP_kliteBlowFishInit
#define kliteCastInit                             ADP_kliteCastInit
#define kliteDESInit                              ADP_kliteDESInit
#define kliteRC2Init                              ADP_kliteRC2Init
#define kliteRC4Init                              ADP_kliteRC4Init
#define kliteMD4Init                              ADP_kliteMD4Init
#define kliteMD5Init                              ADP_kliteMD5Init
#define kliteRMDInit                              ADP_kliteRMDInit
#define kliteSHAInit                              ADP_kliteSHAInit
#define    HAVE_ADP_ERR_load_CRYPTO_strings
#define    HAVE_ADP_CRYPTO_add_lock
#define    HAVE_ADP_CRYPTO_destroy_dynlockid
#define    HAVE_ADP_CRYPTO_get_add_lock_callback
#define    HAVE_ADP_CRYPTO_get_dynlock_create_callback
#define    HAVE_ADP_CRYPTO_get_dynlock_destroy_callback
#define    HAVE_ADP_CRYPTO_get_dynlock_lock_callback
#define    HAVE_ADP_CRYPTO_get_dynlock_value
#define    HAVE_ADP_CRYPTO_get_id_callback
#define    HAVE_ADP_CRYPTO_get_lock_name
#define    HAVE_ADP_CRYPTO_get_locking_callback
#define    HAVE_ADP_CRYPTO_get_new_dynlockid
#define    HAVE_ADP_CRYPTO_get_new_lockid
#define    HAVE_ADP_CRYPTO_lock
#define    HAVE_ADP_CRYPTO_num_locks
#define    HAVE_ADP_CRYPTO_set_add_lock_callback
#define    HAVE_ADP_CRYPTO_set_dynlock_create_callback
#define    HAVE_ADP_CRYPTO_set_dynlock_destroy_callback
#define    HAVE_ADP_CRYPTO_set_dynlock_lock_callback
#define    HAVE_ADP_CRYPTO_set_id_callback
#define    HAVE_ADP_CRYPTO_set_locking_callback
#define    HAVE_ADP_CRYPTO_thread_id
#define    HAVE_ADP_OPENSSL_NONPIC_relocated
#define    HAVE_ADP_OPENSSL_cpuid_setup
#define    HAVE_ADP_OPENSSL_ia32cap_loc
#define    HAVE_ADP_OPENSSL_isservice
#define    HAVE_ADP_OPENSSL_showfatal
#define    HAVE_ADP_OPENSSL_stderr
#define    HAVE_ADP_OpenSSLDie
#define    HAVE_ADP_SSLeay
#define    HAVE_ADP_SSLeay_version
#define    HAVE_ADP_CRYPTO_cleanup_all_ex_data
#define    HAVE_ADP_CRYPTO_dup_ex_data
#define    HAVE_ADP_CRYPTO_ex_data_new_class
#define    HAVE_ADP_CRYPTO_free_ex_data
#define    HAVE_ADP_CRYPTO_get_ex_data
#define    HAVE_ADP_CRYPTO_get_ex_data_implementation
#define    HAVE_ADP_CRYPTO_get_ex_new_index
#define    HAVE_ADP_CRYPTO_new_ex_data
#define    HAVE_ADP_CRYPTO_set_ex_data
#define    HAVE_ADP_CRYPTO_set_ex_data_implementation
#define    HAVE_ADP_CRYPTO_free
#define    HAVE_ADP_CRYPTO_free_locked
#define    HAVE_ADP_CRYPTO_get_mem_functions
#define    HAVE_ADP_CRYPTO_malloc
#define    HAVE_ADP_CRYPTO_malloc_locked
#define    HAVE_ADP_CRYPTO_realloc
#define    HAVE_ADP_CRYPTO_realloc_clean
#define    HAVE_ADP_CRYPTO_set_mem_functions
#define    HAVE_ADP_OPENSSL_cleanse
#define    HAVE_ADP_cleanse_ctr
#define    HAVE_ADP_CRYPTO_is_mem_check_on
#define    HAVE_ADP_CRYPTO_mem_ctrl
#define    HAVE_ADP_CRYPTO_pop_info
#define    HAVE_ADP_CRYPTO_push_info_
#define    HAVE_ADP_OPENSSL_gmtime
#define    HAVE_ADP_OPENSSL_issetugid
#define    HAVE_ADP_AES_cbc_encrypt
#define    HAVE_ADP_AES_cfb128_encrypt
#define    HAVE_ADP_AES_cfb1_encrypt
#define    HAVE_ADP_AES_cfb8_encrypt
#define    HAVE_ADP_AES_cfbr_encrypt_block
#define    HAVE_ADP_AES_decrypt
#define    HAVE_ADP_AES_encrypt
#define    HAVE_ADP_AES_set_decrypt_key
#define    HAVE_ADP_AES_set_encrypt_key
#define    HAVE_ADP_AES_ctr128_encrypt
#define    HAVE_ADP_AES_ecb_encrypt
#define    HAVE_ADP_AES_options
#define    HAVE_ADP_AES_version
#define    HAVE_ADP_AES_ofb128_encrypt
#define    HAVE_ADP_AES_ccm128_decrypt
#define    HAVE_ADP_AES_ccm128_encrypt
#define    HAVE_ADP_AES_ccm128_test
#define    HAVE_ADP_aes_ccmp_debug
#define    HAVE_ADP_AES_bi_ige_encrypt
#define    HAVE_ADP_AES_ige_encrypt
#define    HAVE_ADP_ASN1_BIT_STRING_get_bit
#define    HAVE_ADP_ASN1_BIT_STRING_set
#define    HAVE_ADP_ASN1_BIT_STRING_set_bit
#define    HAVE_ADP_c2i_ASN1_BIT_STRING
#define    HAVE_ADP_i2c_ASN1_BIT_STRING
#define    HAVE_ADP_d2i_ASN1_BOOLEAN
#define    HAVE_ADP_i2d_ASN1_BOOLEAN
#define    HAVE_ADP_d2i_ASN1_bytes
#define    HAVE_ADP_d2i_ASN1_type_bytes
#define    HAVE_ADP_i2d_ASN1_bytes
#define    HAVE_ADP_ASN1_dup
#define    HAVE_ADP_ASN1_item_dup
#define    HAVE_ADP_ASN1_HEADER_free
#define    HAVE_ADP_ASN1_HEADER_new
#define    HAVE_ADP_d2i_ASN1_HEADER
#define    HAVE_ADP_i2d_ASN1_HEADER
#define    HAVE_ADP_ASN1_INTEGER_cmp
#define    HAVE_ADP_ASN1_INTEGER_dup
#define    HAVE_ADP_ASN1_INTEGER_get
#define    HAVE_ADP_ASN1_INTEGER_set
#define    HAVE_ADP_ASN1_INTEGER_to_BN
#define    HAVE_ADP_BN_to_ASN1_INTEGER
#define    HAVE_ADP_c2i_ASN1_INTEGER
#define    HAVE_ADP_d2i_ASN1_UINTEGER
#define    HAVE_ADP_i2c_ASN1_INTEGER
#define    HAVE_ADP_ASN1_OBJECT_create
#define    HAVE_ADP_ASN1_OBJECT_free
#define    HAVE_ADP_ASN1_OBJECT_new
#define    HAVE_ADP_a2d_ASN1_OBJECT
#define    HAVE_ADP_c2i_ASN1_OBJECT
#define    HAVE_ADP_d2i_ASN1_OBJECT
#define    HAVE_ADP_i2a_ASN1_OBJECT
#define    HAVE_ADP_i2d_ASN1_OBJECT
#define    HAVE_ADP_i2t_ASN1_OBJECT
#define    HAVE_ADP_ASN1_OCTET_STRING_cmp
#define    HAVE_ADP_ASN1_OCTET_STRING_dup
#define    HAVE_ADP_ASN1_OCTET_STRING_set
#define    HAVE_ADP_d2i_ASN1_SET
#define    HAVE_ADP_i2d_ASN1_SET
#define    HAVE_ADP_ASN1_TYPE_get
#define    HAVE_ADP_ASN1_TYPE_set
#define    HAVE_ADP_ERR_load_ASN1_strings
#define    HAVE_ADP_ASN1_STRING_cmp
#define    HAVE_ADP_ASN1_STRING_data
#define    HAVE_ADP_ASN1_STRING_dup
#define    HAVE_ADP_ASN1_STRING_free
#define    HAVE_ADP_ASN1_STRING_length
#define    HAVE_ADP_ASN1_STRING_length_set
#define    HAVE_ADP_ASN1_STRING_new
#define    HAVE_ADP_ASN1_STRING_set
#define    HAVE_ADP_ASN1_STRING_type
#define    HAVE_ADP_ASN1_STRING_type_new
#define    HAVE_ADP_ASN1_check_infinite_end
#define    HAVE_ADP_ASN1_const_check_infinite_end
#define    HAVE_ADP_ASN1_get_object
#define    HAVE_ADP_ASN1_object_size
#define    HAVE_ADP_ASN1_put_eoc
#define    HAVE_ADP_ASN1_put_object
#define    HAVE_ADP_ASN1_version
#define    HAVE_ADP_asn1_Finish
#define    HAVE_ADP_asn1_GetSequence
#define    HAVE_ADP_asn1_add_error
#define    HAVE_ADP_asn1_const_Finish
#define    HAVE_ADP_ASN1_parse
#define    HAVE_ADP_ASN1_parse_dump
#define    HAVE_ADP_ASN1_tag2str
#define    HAVE_ADP_ASN1_add_oid_module
#define    HAVE_ADP_d2i_AutoPrivateKey
#define    HAVE_ADP_d2i_PrivateKey
#define    HAVE_ADP_d2i_PublicKey
#define    HAVE_ADP_ASN1_TYPE_get_int_octetstring
#define    HAVE_ADP_ASN1_TYPE_get_octetstring
#define    HAVE_ADP_ASN1_TYPE_set_int_octetstring
#define    HAVE_ADP_ASN1_TYPE_set_octetstring
#define    HAVE_ADP_a2i_ASN1_ENUMERATED
#define    HAVE_ADP_i2a_ASN1_ENUMERATED
#define    HAVE_ADP_a2i_ASN1_INTEGER
#define    HAVE_ADP_i2a_ASN1_INTEGER
#define    HAVE_ADP_a2i_ASN1_STRING
#define    HAVE_ADP_i2a_ASN1_STRING
#define    HAVE_ADP_i2d_PrivateKey
#define    HAVE_ADP_i2d_PublicKey
#define    HAVE_ADP_NETSCAPE_ENCRYPTED_PKEY_free
#define    HAVE_ADP_NETSCAPE_ENCRYPTED_PKEY_it
#define    HAVE_ADP_NETSCAPE_ENCRYPTED_PKEY_new
#define    HAVE_ADP_NETSCAPE_PKEY_free
#define    HAVE_ADP_NETSCAPE_PKEY_it
#define    HAVE_ADP_NETSCAPE_PKEY_new
#define    HAVE_ADP_d2i_NETSCAPE_ENCRYPTED_PKEY
#define    HAVE_ADP_d2i_NETSCAPE_PKEY
#define    HAVE_ADP_d2i_Netscape_RSA
#define    HAVE_ADP_d2i_RSA_NET
#define    HAVE_ADP_i2d_NETSCAPE_ENCRYPTED_PKEY
#define    HAVE_ADP_i2d_NETSCAPE_PKEY
#define    HAVE_ADP_i2d_Netscape_RSA
#define    HAVE_ADP_i2d_RSA_NET
#define    HAVE_ADP_ASN1_item_d2i
#define    HAVE_ADP_ASN1_item_ex_d2i
#define    HAVE_ADP_ASN1_tag2bit
#define    HAVE_ADP_ASN1_template_d2i
#define    HAVE_ADP_asn1_ex_c2i
#define    HAVE_ADP_ASN1_item_ex_i2d
#define    HAVE_ADP_ASN1_item_i2d
#define    HAVE_ADP_ASN1_item_ndef_i2d
#define    HAVE_ADP_ASN1_template_i2d
#define    HAVE_ADP_asn1_ex_i2c
#define    HAVE_ADP_ASN1_item_ex_free
#define    HAVE_ADP_ASN1_item_free
#define    HAVE_ADP_ASN1_primitive_free
#define    HAVE_ADP_ASN1_template_free
#define    HAVE_ADP_ASN1_item_ex_new
#define    HAVE_ADP_ASN1_item_new
#define    HAVE_ADP_ASN1_primitive_new
#define    HAVE_ADP_ASN1_template_new
#define    HAVE_ADP_asn1_primitive_clear
#define    HAVE_ADP_ASN1_ANY_it
#define    HAVE_ADP_ASN1_BIT_STRING_free
#define    HAVE_ADP_ASN1_BIT_STRING_it
#define    HAVE_ADP_ASN1_BIT_STRING_new
#define    HAVE_ADP_ASN1_BMPSTRING_free
#define    HAVE_ADP_ASN1_BMPSTRING_it
#define    HAVE_ADP_ASN1_BMPSTRING_new
#define    HAVE_ADP_ASN1_BOOLEAN_it
#define    HAVE_ADP_ASN1_ENUMERATED_free
#define    HAVE_ADP_ASN1_ENUMERATED_it
#define    HAVE_ADP_ASN1_ENUMERATED_new
#define    HAVE_ADP_ASN1_FBOOLEAN_it
#define    HAVE_ADP_ASN1_GENERALIZEDTIME_free
#define    HAVE_ADP_ASN1_GENERALIZEDTIME_it
#define    HAVE_ADP_ASN1_GENERALIZEDTIME_new
#define    HAVE_ADP_ASN1_GENERALSTRING_free
#define    HAVE_ADP_ASN1_GENERALSTRING_it
#define    HAVE_ADP_ASN1_GENERALSTRING_new
#define    HAVE_ADP_ASN1_IA5STRING_free
#define    HAVE_ADP_ASN1_IA5STRING_it
#define    HAVE_ADP_ASN1_IA5STRING_new
#define    HAVE_ADP_ASN1_INTEGER_free
#define    HAVE_ADP_ASN1_INTEGER_it
#define    HAVE_ADP_ASN1_INTEGER_new
#define    HAVE_ADP_ASN1_NULL_free
#define    HAVE_ADP_ASN1_NULL_it
#define    HAVE_ADP_ASN1_NULL_new
#define    HAVE_ADP_ASN1_OBJECT_it
#define    HAVE_ADP_ASN1_OCTET_STRING_NDEF_it
#define    HAVE_ADP_ASN1_OCTET_STRING_free
#define    HAVE_ADP_ASN1_OCTET_STRING_it
#define    HAVE_ADP_ASN1_OCTET_STRING_new
#define    HAVE_ADP_ASN1_PRINTABLESTRING_free
#define    HAVE_ADP_ASN1_PRINTABLESTRING_it
#define    HAVE_ADP_ASN1_PRINTABLESTRING_new
#define    HAVE_ADP_ASN1_PRINTABLE_free
#define    HAVE_ADP_ASN1_PRINTABLE_it
#define    HAVE_ADP_ASN1_PRINTABLE_new
#define    HAVE_ADP_ASN1_SEQUENCE_it
#define    HAVE_ADP_ASN1_T61STRING_free
#define    HAVE_ADP_ASN1_T61STRING_it
#define    HAVE_ADP_ASN1_T61STRING_new
#define    HAVE_ADP_ASN1_TBOOLEAN_it
#define    HAVE_ADP_ASN1_TYPE_free
#define    HAVE_ADP_ASN1_TYPE_new
#define    HAVE_ADP_ASN1_UNIVERSALSTRING_free
#define    HAVE_ADP_ASN1_UNIVERSALSTRING_it
#define    HAVE_ADP_ASN1_UNIVERSALSTRING_new
#define    HAVE_ADP_ASN1_UTCTIME_free
#define    HAVE_ADP_ASN1_UTCTIME_it
#define    HAVE_ADP_ASN1_UTCTIME_new
#define    HAVE_ADP_ASN1_UTF8STRING_free
#define    HAVE_ADP_ASN1_UTF8STRING_it
#define    HAVE_ADP_ASN1_UTF8STRING_new
#define    HAVE_ADP_ASN1_VISIBLESTRING_free
#define    HAVE_ADP_ASN1_VISIBLESTRING_it
#define    HAVE_ADP_ASN1_VISIBLESTRING_new
#define    HAVE_ADP_DIRECTORYSTRING_free
#define    HAVE_ADP_DIRECTORYSTRING_it
#define    HAVE_ADP_DIRECTORYSTRING_new
#define    HAVE_ADP_DISPLAYTEXT_free
#define    HAVE_ADP_DISPLAYTEXT_it
#define    HAVE_ADP_DISPLAYTEXT_new
#define    HAVE_ADP_d2i_ASN1_BIT_STRING
#define    HAVE_ADP_d2i_ASN1_BMPSTRING
#define    HAVE_ADP_d2i_ASN1_ENUMERATED
#define    HAVE_ADP_d2i_ASN1_GENERALIZEDTIME
#define    HAVE_ADP_d2i_ASN1_GENERALSTRING
#define    HAVE_ADP_d2i_ASN1_IA5STRING
#define    HAVE_ADP_d2i_ASN1_INTEGER
#define    HAVE_ADP_d2i_ASN1_NULL
#define    HAVE_ADP_d2i_ASN1_OCTET_STRING
#define    HAVE_ADP_d2i_ASN1_PRINTABLE
#define    HAVE_ADP_d2i_ASN1_PRINTABLESTRING
#define    HAVE_ADP_d2i_ASN1_T61STRING
#define    HAVE_ADP_d2i_ASN1_TYPE
#define    HAVE_ADP_d2i_ASN1_UNIVERSALSTRING
#define    HAVE_ADP_d2i_ASN1_UTCTIME
#define    HAVE_ADP_d2i_ASN1_UTF8STRING
#define    HAVE_ADP_d2i_ASN1_VISIBLESTRING
#define    HAVE_ADP_d2i_DIRECTORYSTRING
#define    HAVE_ADP_d2i_DISPLAYTEXT
#define    HAVE_ADP_i2d_ASN1_BIT_STRING
#define    HAVE_ADP_i2d_ASN1_BMPSTRING
#define    HAVE_ADP_i2d_ASN1_ENUMERATED
#define    HAVE_ADP_i2d_ASN1_GENERALIZEDTIME
#define    HAVE_ADP_i2d_ASN1_GENERALSTRING
#define    HAVE_ADP_i2d_ASN1_IA5STRING
#define    HAVE_ADP_i2d_ASN1_INTEGER
#define    HAVE_ADP_i2d_ASN1_NULL
#define    HAVE_ADP_i2d_ASN1_OCTET_STRING
#define    HAVE_ADP_i2d_ASN1_PRINTABLE
#define    HAVE_ADP_i2d_ASN1_PRINTABLESTRING
#define    HAVE_ADP_i2d_ASN1_T61STRING
#define    HAVE_ADP_i2d_ASN1_TYPE
#define    HAVE_ADP_i2d_ASN1_UNIVERSALSTRING
#define    HAVE_ADP_i2d_ASN1_UTCTIME
#define    HAVE_ADP_i2d_ASN1_UTF8STRING
#define    HAVE_ADP_i2d_ASN1_VISIBLESTRING
#define    HAVE_ADP_i2d_DIRECTORYSTRING
#define    HAVE_ADP_i2d_DISPLAYTEXT
#define    HAVE_ADP_asn1_do_adb
#define    HAVE_ADP_asn1_do_lock
#define    HAVE_ADP_asn1_enc_free
#define    HAVE_ADP_asn1_enc_init
#define    HAVE_ADP_asn1_enc_restore
#define    HAVE_ADP_asn1_enc_save
#define    HAVE_ADP_asn1_get_choice_selector
#define    HAVE_ADP_asn1_get_field_ptr
#define    HAVE_ADP_asn1_set_choice_selector
#define    HAVE_ADP_DHparams_print
#define    HAVE_ADP_DHparams_print_fp
#define    HAVE_ADP_DSA_print
#define    HAVE_ADP_DSA_print_fp
#define    HAVE_ADP_DSAparams_print
#define    HAVE_ADP_DSAparams_print_fp
#define    HAVE_ADP_RSA_print
#define    HAVE_ADP_RSA_print_fp
#define    HAVE_ADP_X509_ALGOR_dup
#define    HAVE_ADP_X509_ALGOR_free
#define    HAVE_ADP_X509_ALGOR_it
#define    HAVE_ADP_X509_ALGOR_new
#define    HAVE_ADP_d2i_X509_ALGOR
#define    HAVE_ADP_i2d_X509_ALGOR
#define    HAVE_ADP_X509_SIG_free
#define    HAVE_ADP_X509_SIG_it
#define    HAVE_ADP_X509_SIG_new
#define    HAVE_ADP_d2i_X509_SIG
#define    HAVE_ADP_i2d_X509_SIG
#define    HAVE_ADP_BIGNUM_it
#define    HAVE_ADP_CBIGNUM_it
#define    HAVE_ADP_LONG_it
#define    HAVE_ADP_ZLONG_it
#define    HAVE_ADP_BF_cfb64_encrypt
#define    HAVE_ADP_BF_ecb_encrypt
#define    HAVE_ADP_BF_options
#define    HAVE_ADP_BF_version
#define    HAVE_ADP_BF_cbc_encrypt
#define    HAVE_ADP_BF_decrypt
#define    HAVE_ADP_BF_encrypt
#define    HAVE_ADP_BF_ofb64_encrypt
#define    HAVE_ADP_BF_set_key
#define    HAVE_ADP_BIO_dump
#define    HAVE_ADP_BIO_dump_cb
#define    HAVE_ADP_BIO_dump_fp
#define    HAVE_ADP_BIO_dump_indent
#define    HAVE_ADP_BIO_dump_indent_cb
#define    HAVE_ADP_BIO_dump_indent_fp
#define    HAVE_ADP_BIO_printf
#define    HAVE_ADP_BIO_snprintf
#define    HAVE_ADP_BIO_vprintf
#define    HAVE_ADP_BIO_vsnprintf
#define    HAVE_ADP_BIO_accept
#define    HAVE_ADP_BIO_get_accept_socket
#define    HAVE_ADP_BIO_get_host_ip
#define    HAVE_ADP_BIO_get_port
#define    HAVE_ADP_BIO_gethostbyname
#define    HAVE_ADP_BIO_set_tcp_ndelay
#define    HAVE_ADP_BIO_sock_cleanup
#define    HAVE_ADP_BIO_sock_error
#define    HAVE_ADP_BIO_sock_init
#define    HAVE_ADP_BIO_socket_ioctl
#define    HAVE_ADP_BIO_socket_nbio
#define    HAVE_ADP_BIO_f_buffer
#define    HAVE_ADP_BIO_f_linebuffer
#define    HAVE_ADP_BIO_f_nbio_test
#define    HAVE_ADP_BIO_f_null
#define    HAVE_ADP_BIO_debug_callback
#define    HAVE_ADP_ERR_load_BIO_strings
#define    HAVE_ADP_BIO_callback_ctrl
#define    HAVE_ADP_BIO_clear_flags
#define    HAVE_ADP_BIO_copy_next_retry
#define    HAVE_ADP_BIO_ctrl
#define    HAVE_ADP_BIO_ctrl_pending
#define    HAVE_ADP_BIO_ctrl_wpending
#define    HAVE_ADP_BIO_dup_chain
#define    HAVE_ADP_BIO_find_type
#define    HAVE_ADP_BIO_free
#define    HAVE_ADP_BIO_free_all
#define    HAVE_ADP_BIO_get_callback
#define    HAVE_ADP_BIO_get_callback_arg
#define    HAVE_ADP_BIO_get_ex_data
#define    HAVE_ADP_BIO_get_ex_new_index
#define    HAVE_ADP_BIO_get_retry_BIO
#define    HAVE_ADP_BIO_get_retry_reason
#define    HAVE_ADP_BIO_gets
#define    HAVE_ADP_BIO_indent
#define    HAVE_ADP_BIO_int_ctrl
#define    HAVE_ADP_BIO_method_name
#define    HAVE_ADP_BIO_method_type
#define    HAVE_ADP_BIO_new
#define    HAVE_ADP_BIO_next
#define    HAVE_ADP_BIO_number_read
#define    HAVE_ADP_BIO_number_written
#define    HAVE_ADP_BIO_pop
#define    HAVE_ADP_BIO_ptr_ctrl
#define    HAVE_ADP_BIO_push
#define    HAVE_ADP_BIO_puts
#define    HAVE_ADP_BIO_read
#define    HAVE_ADP_BIO_set
#define    HAVE_ADP_BIO_set_callback
#define    HAVE_ADP_BIO_set_callback_arg
#define    HAVE_ADP_BIO_set_ex_data
#define    HAVE_ADP_BIO_set_flags
#define    HAVE_ADP_BIO_test_flags
#define    HAVE_ADP_BIO_vfree
#define    HAVE_ADP_BIO_write
#define    HAVE_ADP_BIO_ACCEPT_free
#define    HAVE_ADP_BIO_ACCEPT_new
#define    HAVE_ADP_BIO_new_accept
#define    HAVE_ADP_BIO_s_accept
#define    HAVE_ADP_BIO_ctrl_get_read_request
#define    HAVE_ADP_BIO_ctrl_get_write_guarantee
#define    HAVE_ADP_BIO_ctrl_reset_read_request
#define    HAVE_ADP_BIO_new_bio_pair
#define    HAVE_ADP_BIO_nread
#define    HAVE_ADP_BIO_nread0
#define    HAVE_ADP_BIO_nwrite
#define    HAVE_ADP_BIO_nwrite0
#define    HAVE_ADP_BIO_s_bio
#define    HAVE_ADP_BIO_CONNECT_free
#define    HAVE_ADP_BIO_CONNECT_new
#define    HAVE_ADP_BIO_new_connect
#define    HAVE_ADP_BIO_s_connect
#define    HAVE_ADP_BIO_fd_non_fatal_error
#define    HAVE_ADP_BIO_fd_should_retry
#define    HAVE_ADP_BIO_new_fd
#define    HAVE_ADP_BIO_s_fd
#define    HAVE_ADP_BIO_new_file
#define    HAVE_ADP_BIO_new_fp
#define    HAVE_ADP_BIO_s_file
#define    HAVE_ADP_BIO_s_log
#define    HAVE_ADP_BIO_new_mem_buf
#define    HAVE_ADP_BIO_s_mem
#define    HAVE_ADP_BIO_s_null
#define    HAVE_ADP_BIO_new_socket
#define    HAVE_ADP_BIO_s_socket
#define    HAVE_ADP_BIO_sock_non_fatal_error
#define    HAVE_ADP_BIO_sock_should_retry
#define    HAVE_ADP_BN_add
#define    HAVE_ADP_BN_sub
#define    HAVE_ADP_BN_uadd
#define    HAVE_ADP_BN_usub
#define    HAVE_ADP_bn_add_words
#define    HAVE_ADP_bn_div_words
#define    HAVE_ADP_bn_mul_add_words
#define    HAVE_ADP_bn_mul_comba4
#define    HAVE_ADP_bn_mul_comba8
#define    HAVE_ADP_bn_mul_words
#define    HAVE_ADP_bn_sqr_comba4
#define    HAVE_ADP_bn_sqr_comba8
#define    HAVE_ADP_bn_sqr_words
#define    HAVE_ADP_bn_sub_words
#define    HAVE_ADP_BN_BLINDING_convert
#define    HAVE_ADP_BN_BLINDING_convert_ex
#define    HAVE_ADP_BN_BLINDING_create_param
#define    HAVE_ADP_BN_BLINDING_free
#define    HAVE_ADP_BN_BLINDING_get_flags
#define    HAVE_ADP_BN_BLINDING_get_thread_id
#define    HAVE_ADP_BN_BLINDING_invert
#define    HAVE_ADP_BN_BLINDING_invert_ex
#define    HAVE_ADP_BN_BLINDING_new
#define    HAVE_ADP_BN_BLINDING_set_flags
#define    HAVE_ADP_BN_BLINDING_set_thread_id
#define    HAVE_ADP_BN_BLINDING_update
#define    HAVE_ADP_BN_CTX_end
#define    HAVE_ADP_BN_CTX_free
#define    HAVE_ADP_BN_CTX_get
#define    HAVE_ADP_BN_CTX_init
#define    HAVE_ADP_BN_CTX_new
#define    HAVE_ADP_BN_CTX_start
#define    HAVE_ADP_BN_div
#define    HAVE_ADP_ERR_load_BN_strings
#define    HAVE_ADP_BN_exp
#define    HAVE_ADP_BN_mod_exp
#define    HAVE_ADP_BN_mod_exp_mont
#define    HAVE_ADP_BN_mod_exp_mont_consttime
#define    HAVE_ADP_BN_mod_exp_mont_word
#define    HAVE_ADP_BN_mod_exp_recp
#define    HAVE_ADP_BN_mod_exp_simple
#define    HAVE_ADP_BN_mod_exp2_mont
#define    HAVE_ADP_BN_gcd
#define    HAVE_ADP_BN_mod_inverse
#define    HAVE_ADP_BN_bin2bn
#define    HAVE_ADP_BN_bn2bin
#define    HAVE_ADP_BN_clear
#define    HAVE_ADP_BN_clear_bit
#define    HAVE_ADP_BN_clear_free
#define    HAVE_ADP_BN_cmp
#define    HAVE_ADP_BN_copy
#define    HAVE_ADP_BN_dup
#define    HAVE_ADP_BN_free
#define    HAVE_ADP_BN_get_params
#define    HAVE_ADP_BN_get_word
#define    HAVE_ADP_BN_init
#define    HAVE_ADP_BN_is_bit_set
#define    HAVE_ADP_BN_mask_bits
#define    HAVE_ADP_BN_new
#define    HAVE_ADP_BN_num_bits
#define    HAVE_ADP_BN_num_bits_word
#define    HAVE_ADP_BN_options
#define    HAVE_ADP_BN_set_bit
#define    HAVE_ADP_BN_set_negative
#define    HAVE_ADP_BN_set_params
#define    HAVE_ADP_BN_set_word
#define    HAVE_ADP_BN_swap
#define    HAVE_ADP_BN_ucmp
#define    HAVE_ADP_BN_value_one
#define    HAVE_ADP_BN_version
#define    HAVE_ADP_bn_cmp_part_words
#define    HAVE_ADP_bn_cmp_words
#define    HAVE_ADP_bn_dup_expand
#define    HAVE_ADP_bn_expand2
#define    HAVE_ADP_BN_mod_add
#define    HAVE_ADP_BN_mod_add_quick
#define    HAVE_ADP_BN_mod_lshift
#define    HAVE_ADP_BN_mod_lshift1
#define    HAVE_ADP_BN_mod_lshift1_quick
#define    HAVE_ADP_BN_mod_lshift_quick
#define    HAVE_ADP_BN_mod_mul
#define    HAVE_ADP_BN_mod_sqr
#define    HAVE_ADP_BN_mod_sub
#define    HAVE_ADP_BN_mod_sub_quick
#define    HAVE_ADP_BN_nnmod
#define    HAVE_ADP_BN_MONT_CTX_copy
#define    HAVE_ADP_BN_MONT_CTX_free
#define    HAVE_ADP_BN_MONT_CTX_init
#define    HAVE_ADP_BN_MONT_CTX_new
#define    HAVE_ADP_BN_MONT_CTX_set
#define    HAVE_ADP_BN_MONT_CTX_set_locked
#define    HAVE_ADP_BN_from_montgomery
#define    HAVE_ADP_BN_mod_mul_montgomery
#define    HAVE_ADP_BN_bn2mpi
#define    HAVE_ADP_BN_mpi2bn
#define    HAVE_ADP_BN_mul
#define    HAVE_ADP_bn_add_part_words
#define    HAVE_ADP_bn_mul_high
#define    HAVE_ADP_bn_mul_low_normal
#define    HAVE_ADP_bn_mul_low_recursive
#define    HAVE_ADP_bn_mul_normal
#define    HAVE_ADP_bn_mul_part_recursive
#define    HAVE_ADP_bn_mul_recursive
#define    HAVE_ADP_bn_sub_part_words
#define    HAVE_ADP_BN_GENCB_call
#define    HAVE_ADP_BN_generate_prime_ex
#define    HAVE_ADP_BN_is_prime_ex
#define    HAVE_ADP_BN_is_prime_fasttest_ex
#define    HAVE_ADP_BN_bn2dec
#define    HAVE_ADP_BN_bn2hex
#define    HAVE_ADP_BN_dec2bn
#define    HAVE_ADP_BN_hex2bn
#define    HAVE_ADP_BN_print
#define    HAVE_ADP_BN_print_fp
#define    HAVE_ADP_BN_bntest_rand
#define    HAVE_ADP_BN_pseudo_rand
#define    HAVE_ADP_BN_pseudo_rand_range
#define    HAVE_ADP_BN_rand
#define    HAVE_ADP_BN_rand_range
#define    HAVE_ADP_BN_RECP_CTX_free
#define    HAVE_ADP_BN_RECP_CTX_init
#define    HAVE_ADP_BN_RECP_CTX_new
#define    HAVE_ADP_BN_RECP_CTX_set
#define    HAVE_ADP_BN_div_recp
#define    HAVE_ADP_BN_mod_mul_reciprocal
#define    HAVE_ADP_BN_reciprocal
#define    HAVE_ADP_BN_lshift
#define    HAVE_ADP_BN_lshift1
#define    HAVE_ADP_BN_rshift
#define    HAVE_ADP_BN_rshift1
#define    HAVE_ADP_BN_sqr
#define    HAVE_ADP_bn_sqr_normal
#define    HAVE_ADP_bn_sqr_recursive
#define    HAVE_ADP_BN_add_word
#define    HAVE_ADP_BN_div_word
#define    HAVE_ADP_BN_mod_word
#define    HAVE_ADP_BN_mul_word
#define    HAVE_ADP_BN_sub_word
#define    HAVE_ADP_BN_mod_sqrt
#define    HAVE_ADP_BN_kronecker
#define    HAVE_ADP_get_rfc2409_prime_1024
#define    HAVE_ADP_get_rfc2409_prime_768
#define    HAVE_ADP_get_rfc3526_prime_1536
#define    HAVE_ADP_get_rfc3526_prime_2048
#define    HAVE_ADP_get_rfc3526_prime_3072
#define    HAVE_ADP_get_rfc3526_prime_4096
#define    HAVE_ADP_get_rfc3526_prime_6144
#define    HAVE_ADP_get_rfc3526_prime_8192
#define    HAVE_ADP_BN_GF2m_add
#define    HAVE_ADP_BN_GF2m_arr2poly
#define    HAVE_ADP_BN_GF2m_mod
#define    HAVE_ADP_BN_GF2m_mod_arr
#define    HAVE_ADP_BN_GF2m_mod_div
#define    HAVE_ADP_BN_GF2m_mod_div_arr
#define    HAVE_ADP_BN_GF2m_mod_exp
#define    HAVE_ADP_BN_GF2m_mod_exp_arr
#define    HAVE_ADP_BN_GF2m_mod_inv
#define    HAVE_ADP_BN_GF2m_mod_inv_arr
#define    HAVE_ADP_BN_GF2m_mod_mul
#define    HAVE_ADP_BN_GF2m_mod_mul_arr
#define    HAVE_ADP_BN_GF2m_mod_solve_quad
#define    HAVE_ADP_BN_GF2m_mod_solve_quad_arr
#define    HAVE_ADP_BN_GF2m_mod_sqr
#define    HAVE_ADP_BN_GF2m_mod_sqr_arr
#define    HAVE_ADP_BN_GF2m_mod_sqrt
#define    HAVE_ADP_BN_GF2m_mod_sqrt_arr
#define    HAVE_ADP_BN_GF2m_poly2arr
#define    HAVE_ADP_BN_get0_nist_prime_192
#define    HAVE_ADP_BN_get0_nist_prime_224
#define    HAVE_ADP_BN_get0_nist_prime_256
#define    HAVE_ADP_BN_get0_nist_prime_384
#define    HAVE_ADP_BN_get0_nist_prime_521
#define    HAVE_ADP_BN_nist_mod_192
#define    HAVE_ADP_BN_nist_mod_224
#define    HAVE_ADP_BN_nist_mod_256
#define    HAVE_ADP_BN_nist_mod_384
#define    HAVE_ADP_BN_nist_mod_521
#define    HAVE_ADP_BN_generate_prime
#define    HAVE_ADP_BN_is_prime
#define    HAVE_ADP_BN_is_prime_fasttest
#define    HAVE_ADP_BUF_MEM_free
#define    HAVE_ADP_BUF_MEM_grow
#define    HAVE_ADP_BUF_MEM_grow_clean
#define    HAVE_ADP_BUF_MEM_new
#define    HAVE_ADP_BUF_memdup
#define    HAVE_ADP_BUF_strdup
#define    HAVE_ADP_BUF_strlcat
#define    HAVE_ADP_BUF_strlcpy
#define    HAVE_ADP_BUF_strndup
#define    HAVE_ADP_ERR_load_BUF_strings
#define    HAVE_ADP_CAST_cfb64_encrypt
#define    HAVE_ADP_CAST_ecb_encrypt
#define    HAVE_ADP_CAST_version
#define    HAVE_ADP_CAST_cbc_encrypt
#define    HAVE_ADP_CAST_decrypt
#define    HAVE_ADP_CAST_encrypt
#define    HAVE_ADP_CAST_ofb64_encrypt
#define    HAVE_ADP_CAST_S_table0
#define    HAVE_ADP_CAST_S_table1
#define    HAVE_ADP_CAST_S_table2
#define    HAVE_ADP_CAST_S_table3
#define    HAVE_ADP_CAST_S_table4
#define    HAVE_ADP_CAST_S_table5
#define    HAVE_ADP_CAST_S_table6
#define    HAVE_ADP_CAST_S_table7
#define    HAVE_ADP_CAST_set_key
#define    HAVE_ADP_COMP_CTX_free
#define    HAVE_ADP_COMP_CTX_new
#define    HAVE_ADP_COMP_compress_block
#define    HAVE_ADP_COMP_expand_block
#define    HAVE_ADP_COMP_rle
#define    HAVE_ADP_COMP_zlib
#define    HAVE_ADP__CONF_add_string
#define    HAVE_ADP__CONF_free_data
#define    HAVE_ADP__CONF_get_section
#define    HAVE_ADP__CONF_get_section_values
#define    HAVE_ADP__CONF_get_string
#define    HAVE_ADP__CONF_new_data
#define    HAVE_ADP__CONF_new_section
#define    HAVE_ADP_CONF_def_version
#define    HAVE_ADP_NCONF_WIN32
#define    HAVE_ADP_NCONF_default
#define    HAVE_ADP_ERR_load_CONF_strings
#define    HAVE_ADP_OPENSSL_load_builtin_modules
#define    HAVE_ADP_CONF_dump_bio
#define    HAVE_ADP_CONF_dump_fp
#define    HAVE_ADP_CONF_free
#define    HAVE_ADP_CONF_get_number
#define    HAVE_ADP_CONF_get_section
#define    HAVE_ADP_CONF_get_string
#define    HAVE_ADP_CONF_load
#define    HAVE_ADP_CONF_load_bio
#define    HAVE_ADP_CONF_load_fp
#define    HAVE_ADP_CONF_set_default_method
#define    HAVE_ADP_CONF_set_nconf
#define    HAVE_ADP_CONF_version
#define    HAVE_ADP_NCONF_dump_bio
#define    HAVE_ADP_NCONF_dump_fp
#define    HAVE_ADP_NCONF_free
#define    HAVE_ADP_NCONF_free_data
#define    HAVE_ADP_NCONF_get_number_e
#define    HAVE_ADP_NCONF_get_section
#define    HAVE_ADP_NCONF_get_string
#define    HAVE_ADP_NCONF_load
#define    HAVE_ADP_NCONF_load_bio
#define    HAVE_ADP_NCONF_load_fp
#define    HAVE_ADP_NCONF_new
#define    HAVE_ADP_CONF_get1_default_config_file
#define    HAVE_ADP_CONF_imodule_get_flags
#define    HAVE_ADP_CONF_imodule_get_module
#define    HAVE_ADP_CONF_imodule_get_name
#define    HAVE_ADP_CONF_imodule_get_usr_data
#define    HAVE_ADP_CONF_imodule_get_value
#define    HAVE_ADP_CONF_imodule_set_flags
#define    HAVE_ADP_CONF_imodule_set_usr_data
#define    HAVE_ADP_CONF_module_add
#define    HAVE_ADP_CONF_module_get_usr_data
#define    HAVE_ADP_CONF_module_set_usr_data
#define    HAVE_ADP_CONF_modules_finish
#define    HAVE_ADP_CONF_modules_free
#define    HAVE_ADP_CONF_modules_load
#define    HAVE_ADP_CONF_modules_load_file
#define    HAVE_ADP_CONF_modules_unload
#define    HAVE_ADP_CONF_parse_list
#define    HAVE_ADP_OPENSSL_config
#define    HAVE_ADP_OPENSSL_no_config
#define    HAVE_ADP_DES_cbc_cksum
#define    HAVE_ADP_DES_cbc_encrypt
#define    HAVE_ADP_DES_ede3_cfb64_encrypt
#define    HAVE_ADP_DES_ede3_cfb_encrypt
#define    HAVE_ADP_DES_cfb64_encrypt
#define    HAVE_ADP_DES_cfb_encrypt
#define    HAVE_ADP_DES_decrypt3
#define    HAVE_ADP_DES_ede3_cbc_encrypt
#define    HAVE_ADP_DES_encrypt1
#define    HAVE_ADP_DES_encrypt2
#define    HAVE_ADP_DES_encrypt3
#define    HAVE_ADP_DES_ncbc_encrypt
#define    HAVE_ADP_DES_ecb3_encrypt
#define    HAVE_ADP_DES_SPtrans
#define    HAVE_ADP_DES_ecb_encrypt
#define    HAVE_ADP_DES_options
#define    HAVE_ADP_OSSL_DES_version
#define    HAVE_ADP_OSSL_libdes_version
#define    HAVE_ADP_DES_enc_read
#define    HAVE_ADP__shadow_DES_rw_mode
#define    HAVE_ADP_DES_enc_write
#define    HAVE_ADP_DES_crypt
#define    HAVE_ADP_DES_fcrypt
#define    HAVE_ADP_fcrypt_body
#define    HAVE_ADP_DES_ede3_ofb64_encrypt
#define    HAVE_ADP_DES_ofb64_encrypt
#define    HAVE_ADP_DES_ofb_encrypt
#define    HAVE_ADP_DES_pcbc_encrypt
#define    HAVE_ADP_DES_quad_cksum
#define    HAVE_ADP_DES_random_key
#define    HAVE_ADP_DES_read_2passwords
#define    HAVE_ADP_DES_read_password
#define    HAVE_ADP__des_crypt
#define    HAVE_ADP_DES_check_key_parity
#define    HAVE_ADP_DES_is_weak_key
#define    HAVE_ADP_DES_key_sched
#define    HAVE_ADP_DES_set_key
#define    HAVE_ADP_DES_set_key_checked
#define    HAVE_ADP_DES_set_key_unchecked
#define    HAVE_ADP_DES_set_odd_parity
#define    HAVE_ADP__shadow_DES_check_key
#define    HAVE_ADP_DES_string_to_2keys
#define    HAVE_ADP_DES_string_to_key
#define    HAVE_ADP_DES_xcbc_encrypt
#define    HAVE_ADP_DES_xwhite_in2out
#define    HAVE_ADP_DHparams_it
#define    HAVE_ADP_d2i_DHparams
#define    HAVE_ADP_i2d_DHparams
#define    HAVE_ADP_DH_check
#define    HAVE_ADP_DH_check_pub_key
#define    HAVE_ADP_ERR_load_DH_strings
#define    HAVE_ADP_DH_generate_parameters_ex
#define    HAVE_ADP_DH_OpenSSL
#define    HAVE_ADP_DH_compute_key
#define    HAVE_ADP_DH_generate_key
#define    HAVE_ADP_DH_free
#define    HAVE_ADP_DH_get_default_method
#define    HAVE_ADP_DH_get_ex_data
#define    HAVE_ADP_DH_get_ex_new_index
#define    HAVE_ADP_DH_new
#define    HAVE_ADP_DH_new_method
#define    HAVE_ADP_DH_set_default_method
#define    HAVE_ADP_DH_set_ex_data
#define    HAVE_ADP_DH_set_method
#define    HAVE_ADP_DH_size
#define    HAVE_ADP_DH_up_ref
#define    HAVE_ADP_DH_version
#define    HAVE_ADP_DH_generate_parameters
#define    HAVE_ADP_DSAPrivateKey_it
#define    HAVE_ADP_DSAPublicKey_it
#define    HAVE_ADP_DSA_SIG_free
#define    HAVE_ADP_DSA_SIG_it
#define    HAVE_ADP_DSA_SIG_new
#define    HAVE_ADP_DSAparams_it
#define    HAVE_ADP_d2i_DSAPrivateKey
#define    HAVE_ADP_d2i_DSAPublicKey
#define    HAVE_ADP_d2i_DSA_SIG
#define    HAVE_ADP_d2i_DSAparams
#define    HAVE_ADP_dsa_pub_internal_it
#define    HAVE_ADP_i2d_DSAPrivateKey
#define    HAVE_ADP_i2d_DSAPublicKey
#define    HAVE_ADP_i2d_DSA_SIG
#define    HAVE_ADP_i2d_DSAparams
#define    HAVE_ADP_ERR_load_DSA_strings
#define    HAVE_ADP_DSA_generate_parameters_ex
#define    HAVE_ADP_DSA_generate_key
#define    HAVE_ADP_DSA_dup_DH
#define    HAVE_ADP_DSA_free
#define    HAVE_ADP_DSA_get_default_method
#define    HAVE_ADP_DSA_get_ex_data
#define    HAVE_ADP_DSA_get_ex_new_index
#define    HAVE_ADP_DSA_new
#define    HAVE_ADP_DSA_new_method
#define    HAVE_ADP_DSA_set_default_method
#define    HAVE_ADP_DSA_set_ex_data
#define    HAVE_ADP_DSA_set_method
#define    HAVE_ADP_DSA_size
#define    HAVE_ADP_DSA_up_ref
#define    HAVE_ADP_DSA_version
#define    HAVE_ADP_DSA_OpenSSL
#define    HAVE_ADP_DSA_do_sign
#define    HAVE_ADP_DSA_sign
#define    HAVE_ADP_DSA_sign_setup
#define    HAVE_ADP_DSA_do_verify
#define    HAVE_ADP_DSA_verify
#define    HAVE_ADP_DSA_generate_parameters
#define    HAVE_ADP_DSO_METHOD_dl
#define    HAVE_ADP_DSO_METHOD_dlfcn
#define    HAVE_ADP_ERR_load_DSO_strings
#define    HAVE_ADP_DSO_bind_func
#define    HAVE_ADP_DSO_bind_var
#define    HAVE_ADP_DSO_convert_filename
#define    HAVE_ADP_DSO_ctrl
#define    HAVE_ADP_DSO_flags
#define    HAVE_ADP_DSO_free
#define    HAVE_ADP_DSO_get_default_method
#define    HAVE_ADP_DSO_get_filename
#define    HAVE_ADP_DSO_get_loaded_filename
#define    HAVE_ADP_DSO_get_method
#define    HAVE_ADP_DSO_load
#define    HAVE_ADP_DSO_merge
#define    HAVE_ADP_DSO_new
#define    HAVE_ADP_DSO_new_method
#define    HAVE_ADP_DSO_set_default_method
#define    HAVE_ADP_DSO_set_filename
#define    HAVE_ADP_DSO_set_method
#define    HAVE_ADP_DSO_set_name_converter
#define    HAVE_ADP_DSO_up_ref
#define    HAVE_ADP_DSO_METHOD_null
#define    HAVE_ADP_DSO_METHOD_openssl
#define    HAVE_ADP_ERR_add_error_data
#define    HAVE_ADP_ERR_clear_error
#define    HAVE_ADP_ERR_error_string
#define    HAVE_ADP_ERR_error_string_n
#define    HAVE_ADP_ERR_free_strings
#define    HAVE_ADP_ERR_func_error_string
#define    HAVE_ADP_ERR_get_err_state_table
#define    HAVE_ADP_ERR_get_error
#define    HAVE_ADP_ERR_get_error_line
#define    HAVE_ADP_ERR_get_error_line_data
#define    HAVE_ADP_ERR_get_implementation
#define    HAVE_ADP_ERR_get_next_error_library
#define    HAVE_ADP_ERR_get_state
#define    HAVE_ADP_ERR_get_string_table
#define    HAVE_ADP_ERR_lib_error_string
#define    HAVE_ADP_ERR_load_ERR_strings
#define    HAVE_ADP_ERR_load_strings
#define    HAVE_ADP_ERR_peek_error
#define    HAVE_ADP_ERR_peek_error_line
#define    HAVE_ADP_ERR_peek_error_line_data
#define    HAVE_ADP_ERR_peek_last_error
#define    HAVE_ADP_ERR_peek_last_error_line
#define    HAVE_ADP_ERR_peek_last_error_line_data
#define    HAVE_ADP_ERR_pop_to_mark
#define    HAVE_ADP_ERR_put_error
#define    HAVE_ADP_ERR_reason_error_string
#define    HAVE_ADP_ERR_release_err_state_table
#define    HAVE_ADP_ERR_remove_state
#define    HAVE_ADP_ERR_set_error_data
#define    HAVE_ADP_ERR_set_implementation
#define    HAVE_ADP_ERR_set_mark
#define    HAVE_ADP_ERR_unload_strings
#define    HAVE_ADP_ERR_print_errors
#define    HAVE_ADP_ERR_print_errors_cb
#define    HAVE_ADP_ERR_print_errors_fp
#define    HAVE_ADP_ERR_load_crypto_strings
#define    HAVE_ADP_pCryptoErrInitRtn
#define    HAVE_ADP_BIO_f_base64
#define    HAVE_ADP_BIO_f_cipher
#define    HAVE_ADP_BIO_set_cipher
#define    HAVE_ADP_BIO_f_md
#define    HAVE_ADP_BIO_f_reliable
#define    HAVE_ADP_EVP_Digest
#define    HAVE_ADP_EVP_DigestFinal
#define    HAVE_ADP_EVP_DigestFinal_ex
#define    HAVE_ADP_EVP_DigestInit
#define    HAVE_ADP_EVP_DigestInit_ex
#define    HAVE_ADP_EVP_DigestUpdate
#define    HAVE_ADP_EVP_MD_CTX_cleanup
#define    HAVE_ADP_EVP_MD_CTX_copy
#define    HAVE_ADP_EVP_MD_CTX_copy_ex
#define    HAVE_ADP_EVP_MD_CTX_create
#define    HAVE_ADP_EVP_MD_CTX_destroy
#define    HAVE_ADP_EVP_MD_CTX_init
#define    HAVE_ADP_EVP_aes_128_cbc
#define    HAVE_ADP_EVP_aes_128_ccm
#define    HAVE_ADP_EVP_aes_128_cfb1
#define    HAVE_ADP_EVP_aes_128_cfb128
#define    HAVE_ADP_EVP_aes_128_cfb8
#define    HAVE_ADP_EVP_aes_128_ecb
#define    HAVE_ADP_EVP_aes_128_ofb
#define    HAVE_ADP_EVP_aes_192_cbc
#define    HAVE_ADP_EVP_aes_192_ccm
#define    HAVE_ADP_EVP_aes_192_cfb1
#define    HAVE_ADP_EVP_aes_192_cfb128
#define    HAVE_ADP_EVP_aes_192_cfb8
#define    HAVE_ADP_EVP_aes_192_ecb
#define    HAVE_ADP_EVP_aes_192_ofb
#define    HAVE_ADP_EVP_aes_256_cbc
#define    HAVE_ADP_EVP_aes_256_ccm
#define    HAVE_ADP_EVP_aes_256_cfb1
#define    HAVE_ADP_EVP_aes_256_cfb128
#define    HAVE_ADP_EVP_aes_256_cfb8
#define    HAVE_ADP_EVP_aes_256_ecb
#define    HAVE_ADP_EVP_aes_256_ofb
#define    HAVE_ADP_EVP_bf_cbc
#define    HAVE_ADP_EVP_bf_cfb64
#define    HAVE_ADP_EVP_bf_ecb
#define    HAVE_ADP_EVP_bf_ofb
#define    HAVE_ADP_EVP_cast5_cbc
#define    HAVE_ADP_EVP_cast5_cfb64
#define    HAVE_ADP_EVP_cast5_ecb
#define    HAVE_ADP_EVP_cast5_ofb
#define    HAVE_ADP_EVP_des_cbc
#define    HAVE_ADP_EVP_des_cfb1
#define    HAVE_ADP_EVP_des_cfb64
#define    HAVE_ADP_EVP_des_cfb8
#define    HAVE_ADP_EVP_des_ecb
#define    HAVE_ADP_EVP_des_ofb
#define    HAVE_ADP_EVP_des_ede
#define    HAVE_ADP_EVP_des_ede3
#define    HAVE_ADP_EVP_des_ede3_cbc
#define    HAVE_ADP_EVP_des_ede3_cfb1
#define    HAVE_ADP_EVP_des_ede3_cfb64
#define    HAVE_ADP_EVP_des_ede3_cfb8
#define    HAVE_ADP_EVP_des_ede3_ecb
#define    HAVE_ADP_EVP_des_ede3_ofb
#define    HAVE_ADP_EVP_des_ede_cbc
#define    HAVE_ADP_EVP_des_ede_cfb64
#define    HAVE_ADP_EVP_des_ede_ecb
#define    HAVE_ADP_EVP_des_ede_ofb
#define    HAVE_ADP_EVP_rc2_40_cbc
#define    HAVE_ADP_EVP_rc2_64_cbc
#define    HAVE_ADP_EVP_rc2_cbc
#define    HAVE_ADP_EVP_rc2_cfb64
#define    HAVE_ADP_EVP_rc2_ecb
#define    HAVE_ADP_EVP_rc2_ofb
#define    HAVE_ADP_EVP_enc_null
#define    HAVE_ADP_EVP_rc4
#define    HAVE_ADP_EVP_rc4_40
#define    HAVE_ADP_EVP_desx_cbc
#define    HAVE_ADP_EVP_DecodeBlock
#define    HAVE_ADP_EVP_DecodeFinal
#define    HAVE_ADP_EVP_DecodeInit
#define    HAVE_ADP_EVP_DecodeUpdate
#define    HAVE_ADP_EVP_EncodeBlock
#define    HAVE_ADP_EVP_EncodeFinal
#define    HAVE_ADP_EVP_EncodeInit
#define    HAVE_ADP_EVP_EncodeUpdate
#define    HAVE_ADP_EVP_CIPHER_CTX_cleanup
#define    HAVE_ADP_EVP_CIPHER_CTX_ctrl
#define    HAVE_ADP_EVP_CIPHER_CTX_free
#define    HAVE_ADP_EVP_CIPHER_CTX_init
#define    HAVE_ADP_EVP_CIPHER_CTX_new
#define    HAVE_ADP_EVP_CIPHER_CTX_rand_key
#define    HAVE_ADP_EVP_CIPHER_CTX_set_key_length
#define    HAVE_ADP_EVP_CIPHER_CTX_set_padding
#define    HAVE_ADP_EVP_CipherFinal
#define    HAVE_ADP_EVP_CipherFinal_ex
#define    HAVE_ADP_EVP_CipherInit
#define    HAVE_ADP_EVP_CipherInit_ex
#define    HAVE_ADP_EVP_CipherUpdate
#define    HAVE_ADP_EVP_DecryptFinal
#define    HAVE_ADP_EVP_DecryptFinal_ex
#define    HAVE_ADP_EVP_DecryptInit
#define    HAVE_ADP_EVP_DecryptInit_ex
#define    HAVE_ADP_EVP_DecryptUpdate
#define    HAVE_ADP_EVP_EncryptFinal
#define    HAVE_ADP_EVP_EncryptFinal_ex
#define    HAVE_ADP_EVP_EncryptInit
#define    HAVE_ADP_EVP_EncryptInit_ex
#define    HAVE_ADP_EVP_EncryptUpdate
#define    HAVE_ADP_EVP_version
#define    HAVE_ADP_ERR_load_EVP_strings
#define    HAVE_ADP_EVP_BytesToKey
#define    HAVE_ADP_EVP_get_pw_prompt
#define    HAVE_ADP_EVP_read_pw_string
#define    HAVE_ADP_EVP_set_pw_prompt
#define    HAVE_ADP_EVP_CIPHER_CTX_block_size
#define    HAVE_ADP_EVP_CIPHER_CTX_cipher
#define    HAVE_ADP_EVP_CIPHER_CTX_flags
#define    HAVE_ADP_EVP_CIPHER_CTX_get_app_data
#define    HAVE_ADP_EVP_CIPHER_CTX_iv_length
#define    HAVE_ADP_EVP_CIPHER_CTX_key_length
#define    HAVE_ADP_EVP_CIPHER_CTX_nid
#define    HAVE_ADP_EVP_CIPHER_CTX_set_app_data
#define    HAVE_ADP_EVP_CIPHER_asn1_to_param
#define    HAVE_ADP_EVP_CIPHER_block_size
#define    HAVE_ADP_EVP_CIPHER_flags
#define    HAVE_ADP_EVP_CIPHER_get_asn1_iv
#define    HAVE_ADP_EVP_CIPHER_iv_length
#define    HAVE_ADP_EVP_CIPHER_key_length
#define    HAVE_ADP_EVP_CIPHER_nid
#define    HAVE_ADP_EVP_CIPHER_param_to_asn1
#define    HAVE_ADP_EVP_CIPHER_set_asn1_iv
#define    HAVE_ADP_EVP_CIPHER_type
#define    HAVE_ADP_EVP_Cipher
#define    HAVE_ADP_EVP_MD_CTX_clear_flags
#define    HAVE_ADP_EVP_MD_CTX_md
#define    HAVE_ADP_EVP_MD_CTX_set_flags
#define    HAVE_ADP_EVP_MD_CTX_test_flags
#define    HAVE_ADP_EVP_MD_block_size
#define    HAVE_ADP_EVP_MD_pkey_type
#define    HAVE_ADP_EVP_MD_size
#define    HAVE_ADP_EVP_MD_type
#define    HAVE_ADP_EVP_PBE_CipherInit
#define    HAVE_ADP_EVP_PBE_alg_add
#define    HAVE_ADP_EVP_PBE_cleanup
#define    HAVE_ADP_EVP_md4
#define    HAVE_ADP_EVP_dss
#define    HAVE_ADP_EVP_dss1
#define    HAVE_ADP_EVP_md5
#define    HAVE_ADP_EVP_md_null
#define    HAVE_ADP_EVP_ripemd160
#define    HAVE_ADP_EVP_sha
#define    HAVE_ADP_EVP_sha1
#define    HAVE_ADP_EVP_sha224
#define    HAVE_ADP_EVP_sha256
#define    HAVE_ADP_EVP_sha384
#define    HAVE_ADP_EVP_sha512
#define    HAVE_ADP_EVP_add_cipher
#define    HAVE_ADP_EVP_add_digest
#define    HAVE_ADP_EVP_cleanup
#define    HAVE_ADP_EVP_get_cipherbyname
#define    HAVE_ADP_EVP_get_digestbyname
#define    HAVE_ADP_EVP_PKEY_decrypt
#define    HAVE_ADP_EVP_PKEY_encrypt
#define    HAVE_ADP_EVP_PKEY_assign
#define    HAVE_ADP_EVP_PKEY_bits
#define    HAVE_ADP_EVP_PKEY_cmp
#define    HAVE_ADP_EVP_PKEY_cmp_parameters
#define    HAVE_ADP_EVP_PKEY_copy_parameters
#define    HAVE_ADP_EVP_PKEY_free
#define    HAVE_ADP_EVP_PKEY_get1_DH
#define    HAVE_ADP_EVP_PKEY_get1_DSA
#define    HAVE_ADP_EVP_PKEY_get1_RSA
#define    HAVE_ADP_EVP_PKEY_missing_parameters
#define    HAVE_ADP_EVP_PKEY_new
#define    HAVE_ADP_EVP_PKEY_save_parameters
#define    HAVE_ADP_EVP_PKEY_set1_DH
#define    HAVE_ADP_EVP_PKEY_set1_DSA
#define    HAVE_ADP_EVP_PKEY_set1_RSA
#define    HAVE_ADP_EVP_PKEY_size
#define    HAVE_ADP_EVP_PKEY_type
#define    HAVE_ADP_EVP_OpenFinal
#define    HAVE_ADP_EVP_OpenInit
#define    HAVE_ADP_EVP_SealFinal
#define    HAVE_ADP_EVP_SealInit
#define    HAVE_ADP_EVP_SignFinal
#define    HAVE_ADP_EVP_VerifyFinal
#define    HAVE_ADP_OPENSSL_add_all_algorithms_conf
#define    HAVE_ADP_EVP_ecdsa
#define    HAVE_ADP_EVP_aes_128_cfb128
#define    HAVE_ADP_EVP_aes_192_cfb128
#define    HAVE_ADP_EVP_aes_256_cfb128
#define    HAVE_ADP_EVP_bf_cfb64
#define    HAVE_ADP_EVP_cast5_cfb64
#define    HAVE_ADP_EVP_des_cfb64
#define    HAVE_ADP_EVP_des_ede3_cfb64
#define    HAVE_ADP_EVP_des_ede_cfb64
#define    HAVE_ADP_EVP_rc2_cfb64
#define    HAVE_ADP_OPENSSL_add_all_algorithms_noconf
#define    HAVE_ADP_OpenSSL_add_all_ciphers
#define    HAVE_ADP_OpenSSL_add_all_digests
#define    HAVE_ADP_HMAC
#define    HAVE_ADP_HMAC_CTX_cleanup
#define    HAVE_ADP_HMAC_CTX_init
#define    HAVE_ADP_HMAC_Final
#define    HAVE_ADP_HMAC_Init
#define    HAVE_ADP_HMAC_Init_ex
#define    HAVE_ADP_HMAC_Update
#define    HAVE_ADP_lh_delete
#define    HAVE_ADP_lh_doall
#define    HAVE_ADP_lh_doall_arg
#define    HAVE_ADP_lh_free
#define    HAVE_ADP_lh_insert
#define    HAVE_ADP_lh_new
#define    HAVE_ADP_lh_num_items
#define    HAVE_ADP_lh_retrieve
#define    HAVE_ADP_lh_strhash
#define    HAVE_ADP_lh_version
#define    HAVE_ADP_lh_node_stats
#define    HAVE_ADP_lh_node_stats_bio
#define    HAVE_ADP_lh_node_usage_stats
#define    HAVE_ADP_lh_node_usage_stats_bio
#define    HAVE_ADP_lh_stats
#define    HAVE_ADP_lh_stats_bio
#define    HAVE_ADP_MD4_Final
#define    HAVE_ADP_MD4_Init
#define    HAVE_ADP_MD4_Transform
#define    HAVE_ADP_MD4_Update
#define    HAVE_ADP_MD4_version
#define    HAVE_ADP_md4_block_data_order
#define    HAVE_ADP_md4_block_host_order
#define    HAVE_ADP_MD4
#define    HAVE_ADP_MD5
#define    HAVE_ADP_MD5_Final
#define    HAVE_ADP_MD5_Init
#define    HAVE_ADP_MD5_Transform
#define    HAVE_ADP_MD5_Update
#define    HAVE_ADP_MD5_version
#define    HAVE_ADP_md5_block_data_order
#define    HAVE_ADP_md5_block_host_order
#define    HAVE_ADP_OBJ_NAME_add
#define    HAVE_ADP_OBJ_NAME_cleanup
#define    HAVE_ADP_OBJ_NAME_do_all
#define    HAVE_ADP_OBJ_NAME_do_all_sorted
#define    HAVE_ADP_OBJ_NAME_get
#define    HAVE_ADP_OBJ_NAME_init
#define    HAVE_ADP_OBJ_NAME_new_index
#define    HAVE_ADP_OBJ_NAME_remove
#define    HAVE_ADP_OBJ_add_object
#define    HAVE_ADP_OBJ_bsearch
#define    HAVE_ADP_OBJ_bsearch_ex
#define    HAVE_ADP_OBJ_cleanup
#define    HAVE_ADP_OBJ_create
#define    HAVE_ADP_OBJ_create_objects
#define    HAVE_ADP_OBJ_ln2nid
#define    HAVE_ADP_OBJ_new_nid
#define    HAVE_ADP_OBJ_nid2ln
#define    HAVE_ADP_OBJ_nid2obj
#define    HAVE_ADP_OBJ_nid2sn
#define    HAVE_ADP_OBJ_obj2nid
#define    HAVE_ADP_OBJ_obj2txt
#define    HAVE_ADP_OBJ_sn2nid
#define    HAVE_ADP_OBJ_txt2nid
#define    HAVE_ADP_OBJ_txt2obj
#define    HAVE_ADP_OBJ_cmp
#define    HAVE_ADP_OBJ_dup
#define    HAVE_ADP_ERR_load_OBJ_strings
#define    HAVE_ADP_ERR_load_PEM_strings
#define    HAVE_ADP_PEM_ASN1_read
#define    HAVE_ADP_PEM_ASN1_write
#define    HAVE_ADP_PEM_ASN1_write_bio
#define    HAVE_ADP_PEM_bytes_read_bio
#define    HAVE_ADP_PEM_def_callback
#define    HAVE_ADP_PEM_dek_info
#define    HAVE_ADP_PEM_do_header
#define    HAVE_ADP_PEM_get_EVP_CIPHER_INFO
#define    HAVE_ADP_PEM_proc_type
#define    HAVE_ADP_PEM_read
#define    HAVE_ADP_PEM_read_bio
#define    HAVE_ADP_PEM_version
#define    HAVE_ADP_PEM_write
#define    HAVE_ADP_PEM_write_bio
#define    HAVE_ADP_PEM_ASN1_read_bio
#define    HAVE_ADP_PEM_SealFinal
#define    HAVE_ADP_PEM_SealInit
#define    HAVE_ADP_PEM_SealUpdate
#define    HAVE_ADP_PEM_read_DHparams
#define    HAVE_ADP_PEM_read_DSAPrivateKey
#define    HAVE_ADP_PEM_read_DSAparams
#define    HAVE_ADP_PEM_read_RSAPrivateKey
#define    HAVE_ADP_PEM_read_RSAPublicKey
#define    HAVE_ADP_PEM_read_bio_DHparams
#define    HAVE_ADP_PEM_read_bio_DSAPrivateKey
#define    HAVE_ADP_PEM_read_bio_DSAparams
#define    HAVE_ADP_PEM_read_bio_RSAPrivateKey
#define    HAVE_ADP_PEM_read_bio_RSAPublicKey
#define    HAVE_ADP_PEM_write_DHparams
#define    HAVE_ADP_PEM_write_DSAPrivateKey
#define    HAVE_ADP_PEM_write_DSAparams
#define    HAVE_ADP_PEM_write_PrivateKey
#define    HAVE_ADP_PEM_write_RSAPrivateKey
#define    HAVE_ADP_PEM_write_RSAPublicKey
#define    HAVE_ADP_PEM_write_bio_DHparams
#define    HAVE_ADP_PEM_write_bio_DSAPrivateKey
#define    HAVE_ADP_PEM_write_bio_DSAparams
#define    HAVE_ADP_PEM_write_bio_PrivateKey
#define    HAVE_ADP_PEM_write_bio_RSAPrivateKey
#define    HAVE_ADP_PEM_write_bio_RSAPublicKey
#define    HAVE_ADP_PEM_read_PrivateKey
#define    HAVE_ADP_PEM_read_bio_PrivateKey
#define    HAVE_ADP_pitem_free
#define    HAVE_ADP_pitem_new
#define    HAVE_ADP_pqueue_find
#define    HAVE_ADP_pqueue_free
#define    HAVE_ADP_pqueue_insert
#define    HAVE_ADP_pqueue_iterator
#define    HAVE_ADP_pqueue_new
#define    HAVE_ADP_pqueue_next
#define    HAVE_ADP_pqueue_peek
#define    HAVE_ADP_pqueue_pop
#define    HAVE_ADP_RAND_SSLeay
#define    HAVE_ADP_RAND_version
#define    HAVE_ADP_rand_ssleay_meth
#define    HAVE_ADP_RAND_egd
#define    HAVE_ADP_RAND_egd_bytes
#define    HAVE_ADP_RAND_query_egd_bytes
#define    HAVE_ADP_ERR_load_RAND_strings
#define    HAVE_ADP_RAND_add
#define    HAVE_ADP_RAND_bytes
#define    HAVE_ADP_RAND_cleanup
#define    HAVE_ADP_RAND_get_rand_method
#define    HAVE_ADP_RAND_pseudo_bytes
#define    HAVE_ADP_RAND_seed
#define    HAVE_ADP_RAND_set_rand_method
#define    HAVE_ADP_RAND_status
#define    HAVE_ADP_adpKlitePRNGInit
#define    HAVE_ADP_klitePRNGExit
#define    HAVE_ADP_klitePRNGGen
#define    HAVE_ADP_klitePRNGSeed
#define    HAVE_ADP_RAND_file_name
#define    HAVE_ADP_RAND_load_file
#define    HAVE_ADP_RAND_write_file
#define    HAVE_ADP_RAND_poll
#define    HAVE_ADP_RC2_cbc_encrypt
#define    HAVE_ADP_RC2_decrypt
#define    HAVE_ADP_RC2_encrypt
#define    HAVE_ADP_RC2_ecb_encrypt
#define    HAVE_ADP_RC2_version
#define    HAVE_ADP_RC2_set_key
#define    HAVE_ADP_RC2_cfb64_encrypt
#define    HAVE_ADP_RC2_ofb64_encrypt
#define    HAVE_ADP_RC4
#define    HAVE_ADP_RC4_options
#define    HAVE_ADP_RC4_set_key
#define    HAVE_ADP_RC4_version
#define    HAVE_ADP_RIPEMD160_Final
#define    HAVE_ADP_RIPEMD160_Init
#define    HAVE_ADP_RIPEMD160_Transform
#define    HAVE_ADP_RIPEMD160_Update
#define    HAVE_ADP_RMD160_version
#define    HAVE_ADP_RIPEMD160
#define    HAVE_ADP_RSAPrivateKey_asn1_meth
#define    HAVE_ADP_RSAPrivateKey_dup
#define    HAVE_ADP_RSAPrivateKey_it
#define    HAVE_ADP_RSAPublicKey_dup
#define    HAVE_ADP_RSAPublicKey_it
#define    HAVE_ADP_d2i_RSAPrivateKey
#define    HAVE_ADP_d2i_RSAPublicKey
#define    HAVE_ADP_i2d_RSAPrivateKey
#define    HAVE_ADP_i2d_RSAPublicKey
#define    HAVE_ADP_RSA_check_key
#define    HAVE_ADP_RSA_PKCS1_SSLeay
#define    HAVE_ADP_ERR_load_RSA_strings
#define    HAVE_ADP_RSA_generate_key_ex
#define    HAVE_ADP_RSA_blinding_off
#define    HAVE_ADP_RSA_blinding_on
#define    HAVE_ADP_RSA_flags
#define    HAVE_ADP_RSA_free
#define    HAVE_ADP_RSA_get_default_method
#define    HAVE_ADP_RSA_get_ex_data
#define    HAVE_ADP_RSA_get_ex_new_index
#define    HAVE_ADP_RSA_get_method
#define    HAVE_ADP_RSA_memory_lock
#define    HAVE_ADP_RSA_new
#define    HAVE_ADP_RSA_new_method
#define    HAVE_ADP_RSA_private_decrypt
#define    HAVE_ADP_RSA_private_encrypt
#define    HAVE_ADP_RSA_public_decrypt
#define    HAVE_ADP_RSA_public_encrypt
#define    HAVE_ADP_RSA_set_default_method
#define    HAVE_ADP_RSA_set_ex_data
#define    HAVE_ADP_RSA_set_method
#define    HAVE_ADP_RSA_setup_blinding
#define    HAVE_ADP_RSA_size
#define    HAVE_ADP_RSA_up_ref
#define    HAVE_ADP_RSA_version
#define    HAVE_ADP_RSA_padding_add_none
#define    HAVE_ADP_RSA_padding_check_none
#define    HAVE_ADP_RSA_null_method
#define    HAVE_ADP_MGF1
#define    HAVE_ADP_PKCS1_MGF1
#define    HAVE_ADP_RSA_padding_add_PKCS1_OAEP
#define    HAVE_ADP_RSA_padding_check_PKCS1_OAEP
#define    HAVE_ADP_RSA_padding_add_PKCS1_type_1
#define    HAVE_ADP_RSA_padding_add_PKCS1_type_2
#define    HAVE_ADP_RSA_padding_check_PKCS1_type_1
#define    HAVE_ADP_RSA_padding_check_PKCS1_type_2
#define    HAVE_ADP_RSA_sign_ASN1_OCTET_STRING
#define    HAVE_ADP_RSA_verify_ASN1_OCTET_STRING
#define    HAVE_ADP_RSA_sign
#define    HAVE_ADP_RSA_verify
#define    HAVE_ADP_RSA_padding_add_SSLv23
#define    HAVE_ADP_RSA_padding_check_SSLv23
#define    HAVE_ADP_RSA_X931_hash_id
#define    HAVE_ADP_RSA_padding_add_X931
#define    HAVE_ADP_RSA_padding_check_X931
#define    HAVE_ADP_RSA_generate_key
#define    HAVE_ADP_SHA1_Final
#define    HAVE_ADP_SHA1_Init
#define    HAVE_ADP_SHA1_Transform
#define    HAVE_ADP_SHA1_Update
#define    HAVE_ADP_SHA1_version
#define    HAVE_ADP_sha1_block_data_order
#define    HAVE_ADP_sha1_block_host_order
#define    HAVE_ADP_SHA1
#define    HAVE_ADP_SHA_Final
#define    HAVE_ADP_SHA_Init
#define    HAVE_ADP_SHA_Transform
#define    HAVE_ADP_SHA_Update
#define    HAVE_ADP_SHA_version
#define    HAVE_ADP_sha_block_data_order
#define    HAVE_ADP_sha_block_host_order
#define    HAVE_ADP_SHA
#define    HAVE_ADP_SHA224
#define    HAVE_ADP_SHA224_Final
#define    HAVE_ADP_SHA224_Init
#define    HAVE_ADP_SHA224_Update
#define    HAVE_ADP_SHA256
#define    HAVE_ADP_SHA256_Final
#define    HAVE_ADP_SHA256_Init
#define    HAVE_ADP_SHA256_Transform
#define    HAVE_ADP_SHA256_Update
#define    HAVE_ADP_SHA256_version
#define    HAVE_ADP_sha256_block_data_order
#define    HAVE_ADP_sha256_block_host_order
#define    HAVE_ADP_SHA384
#define    HAVE_ADP_SHA384_Final
#define    HAVE_ADP_SHA384_Init
#define    HAVE_ADP_SHA384_Update
#define    HAVE_ADP_SHA512
#define    HAVE_ADP_SHA512_Final
#define    HAVE_ADP_SHA512_Init
#define    HAVE_ADP_SHA512_Transform
#define    HAVE_ADP_SHA512_Update
#define    HAVE_ADP_SHA512_version
#define    HAVE_ADP_STACK_version
#define    HAVE_ADP_sk_delete
#define    HAVE_ADP_sk_delete_ptr
#define    HAVE_ADP_sk_dup
#define    HAVE_ADP_sk_find
#define    HAVE_ADP_sk_find_ex
#define    HAVE_ADP_sk_free
#define    HAVE_ADP_sk_insert
#define    HAVE_ADP_sk_is_sorted
#define    HAVE_ADP_sk_new
#define    HAVE_ADP_sk_new_null
#define    HAVE_ADP_sk_num
#define    HAVE_ADP_sk_pop
#define    HAVE_ADP_sk_pop_free
#define    HAVE_ADP_sk_push
#define    HAVE_ADP_sk_set
#define    HAVE_ADP_sk_set_cmp_func
#define    HAVE_ADP_sk_shift
#define    HAVE_ADP_sk_sort
#define    HAVE_ADP_sk_unshift
#define    HAVE_ADP_sk_value
#define    HAVE_ADP_sk_zero
#define    HAVE_ADP_TXT_DB_create_index
#define    HAVE_ADP_TXT_DB_free
#define    HAVE_ADP_TXT_DB_get_by_index
#define    HAVE_ADP_TXT_DB_insert
#define    HAVE_ADP_TXT_DB_read
#define    HAVE_ADP_TXT_DB_version
#define    HAVE_ADP_TXT_DB_write
#define    HAVE_ADP__ossl_old_des_read_pw
#define    HAVE_ADP__ossl_old_des_read_pw_string
#define    HAVE_ADP_ERR_load_UI_strings
#define    HAVE_ADP_UI_add_error_string
#define    HAVE_ADP_UI_add_info_string
#define    HAVE_ADP_UI_add_input_boolean
#define    HAVE_ADP_UI_add_input_string
#define    HAVE_ADP_UI_add_user_data
#define    HAVE_ADP_UI_add_verify_string
#define    HAVE_ADP_UI_construct_prompt
#define    HAVE_ADP_UI_create_method
#define    HAVE_ADP_UI_ctrl
#define    HAVE_ADP_UI_destroy_method
#define    HAVE_ADP_UI_dup_error_string
#define    HAVE_ADP_UI_dup_info_string
#define    HAVE_ADP_UI_dup_input_boolean
#define    HAVE_ADP_UI_dup_input_string
#define    HAVE_ADP_UI_dup_verify_string
#define    HAVE_ADP_UI_free
#define    HAVE_ADP_UI_get0_action_string
#define    HAVE_ADP_UI_get0_output_string
#define    HAVE_ADP_UI_get0_result
#define    HAVE_ADP_UI_get0_result_string
#define    HAVE_ADP_UI_get0_test_string
#define    HAVE_ADP_UI_get0_user_data
#define    HAVE_ADP_UI_get_default_method
#define    HAVE_ADP_UI_get_ex_data
#define    HAVE_ADP_UI_get_ex_new_index
#define    HAVE_ADP_UI_get_input_flags
#define    HAVE_ADP_UI_get_method
#define    HAVE_ADP_UI_get_result_maxsize
#define    HAVE_ADP_UI_get_result_minsize
#define    HAVE_ADP_UI_get_string_type
#define    HAVE_ADP_UI_method_get_closer
#define    HAVE_ADP_UI_method_get_flusher
#define    HAVE_ADP_UI_method_get_opener
#define    HAVE_ADP_UI_method_get_reader
#define    HAVE_ADP_UI_method_get_writer
#define    HAVE_ADP_UI_method_set_closer
#define    HAVE_ADP_UI_method_set_flusher
#define    HAVE_ADP_UI_method_set_opener
#define    HAVE_ADP_UI_method_set_reader
#define    HAVE_ADP_UI_method_set_writer
#define    HAVE_ADP_UI_new
#define    HAVE_ADP_UI_new_method
#define    HAVE_ADP_UI_process
#define    HAVE_ADP_UI_set_default_method
#define    HAVE_ADP_UI_set_ex_data
#define    HAVE_ADP_UI_set_method
#define    HAVE_ADP_UI_set_result
#define    HAVE_ADP_UI_OpenSSL
#define    HAVE_ADP_UI_UTIL_read_pw
#define    HAVE_ADP_UI_UTIL_read_pw_string
#define    HAVE_ADP_opensslCryptoApiLibInit
#define    HAVE_ADP_opensslCryptoLockInstall
#define    HAVE_ADP_opensslCryptoLockUnInstall
#define    HAVE_ADP_cryptoPartIdInit
#define    HAVE_ADP_cryptoSupportMemInit
#define    HAVE_ADP_kliteEngineErrorTableInit
#define    HAVE_ADP_kliteErrorTableInit
#define    HAVE_ADP_kliteX509ErrorTableInit
#define    HAVE_ADP_PKCS12_PBE_add
#define    HAVE_ADP_PKCS5_PBE_add
#define    HAVE_ADP_SSL_library_init
#define    HAVE_ADP_SSL_load_error_strings
#define    HAVE_ADP_X509_get_default_cert_area
#define    HAVE_ADP_kliteInitStatus
#define    HAVE_ADP_kliteLibInit
#define    HAVE_ADP_kliteAESInit
#define    HAVE_ADP_kliteBlowFishInit
#define    HAVE_ADP_kliteCastInit
#define    HAVE_ADP_kliteDESInit
#define    HAVE_ADP_kliteRC2Init
#define    HAVE_ADP_kliteRC4Init
#define    HAVE_ADP_kliteMD4Init
#define    HAVE_ADP_kliteMD5Init
#define    HAVE_ADP_kliteRMDInit
#define    HAVE_ADP_kliteSHAInit

#if	defined(__i386)   || defined(__i386__)   || defined(_M_IX86) || \
	defined(__INTEL__) || \
	defined(__x86_64) || defined(__x86_64__) || defined(_M_AMD64)

#define  OPENSSL_ia32cap_P                             ADP_OPENSSL_ia32cap_P
#define  HAVE_ADP_OPENSSL_ia32cap_P
#endif 

#if defined(__i386) || defined(__i386__) || defined(_M_IX86) || defined(__INTEL__)
# if !defined(B_ENDIAN)
#undef md4_block_data_order
#undef HAVE_ADP_md4_block_data_order
#undef md5_block_data_order
#undef HAVE_ADP_md5_block_data_order
#undef ripemd160_block_data_order
#undef HAVE_ADP_ripemd160_block_data_order
#endif 
#endif


#define __INCadpKliteGlobalMapH
#endif /* __INCadpKliteGlobalMapH */

